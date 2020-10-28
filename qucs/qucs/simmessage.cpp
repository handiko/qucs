/***************************************************************************
                              simmessage.cpp
                             ----------------
    begin                : Sat Sep 6 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*!
 * \file simmessage.cpp
 * \brief Simulation dialog implementation
 *
 */

#include <stdlib.h>
#include <iostream>
using namespace std;
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QRegExp>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QProgressBar>
#include <QDebug>
#include <QMessageBox>
#include <QTextBlock>

#include "simmessage.h"
#include "module.h"
#include "qucs.h"
#include "textdoc.h"
#include "schematic_doc.h"
#include "components/opt_sim.h"
#include "components/vhdlfile.h"
#include "misc.h"
#include "globals.h"
#include "docfmt.h"

/*!
 * \brief Create a simulation messages dialog.
 *
 *  Create a new dialog to show the simulation steps progress and the
 *  simulator output messages
 */
SimMessage::SimMessage(QWidget *w, QWidget *parent)
		: QDialog(parent) 
{
  setWindowTitle(tr("Qucs Simulation Messages"));
  QucsDoc *Doc = prechecked_cast<QucsDoc*>(w);
  assert(Doc);
  DocWidget = w;

  DocName = Doc->docName();
  DataDisplay = Doc->DataDisplay;
  Script = Doc->Script;
  QFileInfo Info(DocName);
  DataSet = QDir::toNativeSeparators(Info.path()) +
    QDir::separator() + Doc->DataSet;
  showBias = Doc->showBias;     // save some settings as the document...
  SimOpenDpl = Doc->SimOpenDpl; // ...could be closed during the simulation.
  SimRunScript = Doc->SimRunScript;

  all = new QVBoxLayout(this);
  all->setSpacing(5);
  all->setMargin(5);
  QGroupBox *Group1 = new QGroupBox(tr("Progress:"));
  all->addWidget(Group1);
  QVBoxLayout *vbox1 = new QVBoxLayout();
  Group1->setLayout(vbox1);

  ProgText = new QPlainTextEdit();
  vbox1->addWidget(ProgText);
  ProgText->setReadOnly(true);
  //ProgText->setWordWrapMode(QTextOption::NoWrap);
  ProgText->setMinimumSize(400,80);
  wasLF = false;
  simKilled = false;

  QGroupBox *HGroup = new QGroupBox();
  QHBoxLayout *hbox = new QHBoxLayout();
  //HGroup->setInsideMargin(5);
  //HGroup->setInsideSpacing(5);
  all->addWidget(HGroup);
  QLabel *progr = new QLabel(tr("Progress:"));
  hbox->addWidget(progr);
  SimProgress = new QProgressBar();
  hbox->addWidget(SimProgress);
//  SimProgress->setPercentageVisible(false);
  HGroup->setLayout(hbox);

  QGroupBox *Group2 = new QGroupBox(tr("Errors and Warnings:"));
  all->addWidget(Group2);
  QVBoxLayout *vbox2 = new QVBoxLayout();

  ErrText = new QPlainTextEdit();
  vbox2->addWidget(ErrText);
  ErrText->setReadOnly(true);
  ErrText->setWordWrapMode(QTextOption::NoWrap);
  ErrText->setMinimumSize(400,80);
  Group2->setLayout(vbox2);

  QHBoxLayout *Butts = new QHBoxLayout();
  all->addLayout(Butts);

  Display = new QPushButton(tr("Goto display page"));
  Butts->addWidget(Display);
  Display->setDisabled(true);
  connect(Display,SIGNAL(clicked()),SLOT(slotDisplayButton()));

  Abort = new QPushButton(tr("Abort simulation"));
  Butts->addWidget(Abort);
  // Abort will close the window, which in turn will abort the simulation
  connect(Abort,SIGNAL(clicked()),SLOT(reject()));
  connect(this,SIGNAL(rejected()),SLOT(AbortSim()));
}

SimMessage::~SimMessage()
{
  if(SimProcess.state()==QProcess::Running)  SimProcess.kill();
  delete all;
}

// ------------------------------------------------------------------------
bool SimMessage::startProcess()
{
  Abort->setText(tr("Abort simulation"));
  Display->setDisabled(true);

  QString txt = tr("Starting new simulation on %1 at %2").
    arg(QDate::currentDate().toString("ddd dd. MMM yyyy")).
    arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
  ProgText->appendPlainText(txt + "\n");

  SimProcess.blockSignals(false);
 /* On Qt4 it shows as running even before we .start it. FIXME*/
  if(SimProcess.state()==QProcess::Running ||SimProcess.state()==QProcess::Starting) { untested();
    qDebug() << "running";
    ErrText->appendPlainText(tr("ERROR: Simulator is still running"));
    FinishSimulation(-1);
    return false;
  }else{itested();
  }

  Collect.clear();  // clear list for NodeSets, SPICE components etc.
  ProgText->appendPlainText(tr("creating netlist... "));
  NetlistFile.setFileName(QucsSettings.QucsHomeDir.filePath("netlist.txt"));

  // BUG: ask simulator driver
  auto dl = command_dispatcher["legacy_nl"];
  assert(dl);
  DocumentFormat const* n = prechecked_cast<DocumentFormat const*>(dl);
  assert(n);

  auto Doc = dynamic_cast<QucsDoc const*>(DocWidget);

  if(QucsApp::isTextDocument(Doc)) { untested();
    incomplete();
    // throw(Error(" Cannot simulate a text file");
  }else if(SchematicDoc const* d=dynamic_cast<SchematicDoc const*>(DocWidget)){itested();
    assert(d->root());
    trace1("save", dl);

    /// BUG. previously, there was an obsolete "prepareNetlist" call in "startProcess". ///
    // n->save(Stream, *d->root());
  // auto& nl=*n;
//    SimPorts = ((SchematicDoc*)DocWidget)->prepareNetlist(Stream, Collect, ErrText, nl);
  }else{ untested();
    incomplete();
  }

#if 0 //???
  if(SimPorts < -5) {
    NetlistFile.close();
    ErrText->appendPlainText(tr("ERROR: Cannot simulate a text file!"));
    FinishSimulation(-1);
    return false;
  }
#endif

  // Collect.append("*");   // mark the end??
  //
  // simulator->init(doc)??

  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()),
                       SLOT(slotReadSpiceNetlist()));
  connect(&SimProcess, SIGNAL(finished(int)),
                       SLOT(slotFinishSpiceNetlist(int)));

//  nextSPICE(); /// ???
  startSimulator();
  return true;
  // Since now, the Doc pointer may be obsolete, as the user could have
  // closed the schematic !!!
}

// BUG: must create netlist in netlister.
void SimMessage::nextSPICE()
{ untested();

#if 0
  incomplete();
  QString Line;
  for(;;) {  // search for next SPICE component
    Line = *(Collect.begin());
    Collect.erase(Collect.begin());
    if(Line == "*") {  // worked on all components ?
      startSimulator(); // <<<<<================== go on ===
      return;
    }
// FIXME #warning SPICE section below not being covered?
    qDebug() << "goin thru SPICE branch on simmmessage.cpp";
    if(Line.left(5) == "SPICE") {
      if(Line.at(5) != 'o') insertSim = true;
      else insertSim = false;
      break;
    }
    Collect.append(Line);
  }


  QString FileName = Line.section('"', 1,1);
  Line = Line.section('"', 2);  // port nodes
  if(Line.isEmpty()){
    makeSubcircuit = false;
  } else{
    makeSubcircuit = true;
  }

  QString prog;
  QStringList com;
  prog = QucsSettings.Qucsconv;
  if(makeSubcircuit){
    com << "-g" << "_ref";
  }else{
  }
  com << "-if" << "spice" << "-of" << "qucs";

  QFile SpiceFile;
  if(FileName.indexOf(QDir::separator()) < 0){
    // add path ?
    SpiceFile.setFileName(QucsSettings.QucsWorkDir.path() + QDir::separator() + FileName);
  }else{
    SpiceFile.setFileName(FileName);
  }

  if(!SpiceFile.open(QIODevice::ReadOnly)) {
    ErrText->appendPlainText(tr("ERROR: Cannot open SPICE file \"%1\".").arg(FileName));
    FinishSimulation(-1);
    return;
  }else{
  }

  if(makeSubcircuit) {
    Stream << "\n.Def:" << misc::properName(FileName) << " ";

    Line.replace(',', ' ');
    Stream << Line;
    if(!Line.isEmpty()) Stream << " _ref";
  }else{
  }
  Stream << "\n";

  ProgressText = "";

  qDebug() << "start QucsConv" << prog << com.join(" ");
  SimProcess.start(prog, com);

  if(SimProcess.state() != QProcess::Running) {
    ErrText->appendPlainText(tr("SIM ERROR: Cannot start QucsConv!"));
    FinishSimulation(-1);
    return;
  }else{
  }

  QByteArray SpiceContent = SpiceFile.readAll();
  SpiceFile.close();
  QString command(SpiceContent); //to convert byte array to string
  SimProcess.setStandardInputFile(command);  //? FIXME works?
  qDebug() << command;
  connect(&SimProcess, SIGNAL(wroteToStdin()), SLOT(slotCloseStdin()));
#endif
}

// ------------------------------------------------------------------------
void SimMessage::slotCloseStdin()
{
  //SimProcess.closeStdin(); //? channel not available in Qt4?
  disconnect(&SimProcess, SIGNAL(wroteToStdin()), 0, 0);
}

// ------------------------------------------------------------------------
void SimMessage::slotReadSpiceNetlist()
{
#if 0
  int i;
  QString s;
  ProgressText += QString(SimProcess.readAllStandardOutput());

  while((i = ProgressText.indexOf('\n')) >= 0) {

    s = ProgressText.left(i);
    ProgressText.remove(0, i+1);


    s = s.trimmed();
    if(s.isEmpty()) continue;
    if(s.at(0) == '#') continue;
    if(s.at(0) == '.') if(s.left(5) != ".Def:") { // insert simulations later
      if(insertSim) Collect.append(s);
      continue;
    }
    Stream << "  " << s << '\n';
  }
#endif
}

// ------------------------------------------------------------------------
void SimMessage::slotFinishSpiceNetlist(int)
{
#if 0
  Q_UNUSED(status);

  if(makeSubcircuit){ untested();
    Stream << ".Def:End\n\n";
  }else{ untested();
  }

  nextSPICE();
#endif
}

// ------------------------------------------------------------------------
#ifdef __MINGW32__ // -> platform.h
#include <windows.h>
static QString pathName(QString longpath) {
  const char * lpath = QDir::toNativeSeparators(longpath).toAscii();
  char spath[2048];
  int len = GetShortPathNameA(lpath,spath,sizeof(spath)-1);
  spath[len] = '\0';
  return QString(spath);
}
#else
static QString pathName(QString longpath) {
  return longpath;
}
#endif

// "run" simulator?
void SimMessage::startSimulator()
{
  // TODO: let user choose
  Simulator const* proto=simulator_dispatcher["legacy"];
  Simulator* sim = proto->clone();

  { // not here

    auto qucsdoc = prechecked_cast<QucsDoc*>(DocWidget);
    assert(qucsdoc);

    sim->attachDoc(qucsdoc); // TODO: disallow closing schematic as long as simulator is attached.
  }

    // wrong place.
//    isVerilog = ((SchematicDoc*)DocWidget)->isVerilog;

  QString SimTime;
  QStringList Arguments;
  QString SimPath = QDir::toNativeSeparators(QucsSettings.QucsHomeDir.absolutePath());
#ifdef __MINGW32__ // -> platform.h
  QString QucsDigiLib = "qucsdigilib.bat";
  QString QucsDigi = "qucsdigi.bat";
  QString QucsVeri = "qucsveri.bat";
#else
  QString QucsDigiLib = "qucsdigilib";
  QString QucsDigi = "qucsdigi";
  QString QucsVeri = "qucsveri";
#endif
  SimOpt = NULL;
  bool isVerilog = false;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  if(TextDoc const* Doc=dynamic_cast<TextDoc const*>(DocWidget)){ untested();
    incomplete();

    // Take VHDL file in memory as it could contain unsaved changes.
    // Stream << Doc->toPlainText();
    NetlistFile.close();
    ProgText->insertPlainText(tr("done.\n"));  // of "creating netlist...

    if (Doc->simulation) {
      SimTime = Doc->SimTime;

      QString libs = Doc->Libraries.toLower();
#ifdef __MINGW32__ // -> platform.h
      if(libs.isEmpty()) {
        libs = "";
      } else {
        libs.replace(" ",",-l");
        libs = "-Wl,-l" + libs;
      }
#else
      if(libs.isEmpty()) {
        libs = "-c";
      } else {
        libs.replace(" ",",-l");
        libs = "-c,-l" + libs;
      }
#endif

      Program = pathName(QucsSettings.BinDir + QucsDigi);
      Arguments  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                 << DataSet << SimTime << pathName(SimPath)
                 << pathName(QucsSettings.BinDir) << libs;
    }else{ // no Doc->simulation, but textDoc
      incomplete();
#if 0
      QString text = Doc->toPlainText();
      VHDL_File_Info VInfo (text);
      QString entity = VInfo.EntityName.toLower();
      QString lib = Doc->Library.toLower();
      if (lib.isEmpty()) lib = "work";
      QString dir = QDir::toNativeSeparators(QucsSettings.QucsHomeDir.path());
      QDir vhdlDir(dir);
      if(!vhdlDir.exists("vhdl"))
	if(!vhdlDir.mkdir("vhdl")) {
	  ErrText->appendPlainText(tr("ERROR: Cannot create VHDL directory \"%1\"!")
			  .arg(vhdlDir.path()+"/vhdl"));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/vhdl");
      if(!vhdlDir.exists(lib))
	if(!vhdlDir.mkdir(lib)) {
	  ErrText->appendPlainText(tr("ERROR: Cannot create VHDL directory \"%1\"!")
			  .arg(vhdlDir.path()+"/"+lib));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/"+lib);
      QFile destFile;
      destFile.setFileName(vhdlDir.filePath(entity+".vhdl"));
      if(!destFile.open(QIODevice::WriteOnly)) {
	ErrText->appendPlainText(tr("ERROR: Cannot create \"%1\"!")
			.arg(destFile.fileName()));
	return;
      }
      destFile.write(text.toAscii(), text.length());
      destFile.close();
      Program = pathName(QucsSettings.BinDir + QucsDigiLib);
      Arguments << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                << pathName(SimPath)
                << entity
                << lib;
#endif
    }
  }else if(SchematicDoc const* d=dynamic_cast<SchematicDoc const*>(DocWidget)){itested();
    incomplete();


    assert(sim); //for now.
    // BUG: ask simulator driver
    auto dl = sim->netLister();
    assert(dl);
    DocumentFormat const* n = prechecked_cast<DocumentFormat const*>(dl);
    assert(n);

//    SimTime = d->createNetlist(Stream, SimPorts, *nl);
//
//    try{
      ostream_t Stream(&NetlistFile);
      n->save(Stream, *d->root());
//    }catch(...){
//      ErrText->appendPlainText(tr("ERROR: Cannot write netlist file!"));
//      FinishSimulation(-1);
//      incomplete();
//      return false;
//    }


    NetLang const* nl = sim->netLang();

    for(auto c : d->commands()){
      nl->printItem(c, Stream);
    }

    if(SimTime.length()>0&&SimTime.at(0) == '\xA7') {
    //  NetlistFile.close();
      ErrText->insertPlainText(SimTime.mid(1));
      FinishSimulation(-1);
      return;
    }
#if 0
    if (isVerilog) {
      Stream << "\n"
	     << "  initial begin\n"
	     << "    $dumpfile(\"digi.vcd\");\n"
	     << "    $dumpvars();\n"
	     << "    #" << SimTime << " $finish;\n"
	     << "  end\n\n"
	     << "endmodule // TestBench\n";
    }
#endif
    NetlistFile.close();
    ProgText->insertPlainText(tr("done.\n"));  // of "creating netlist...

    if(1 || SimPorts < 0) {

      // append command arguments
      // append netlist with same arguments
      if (Module::vaComponents.isEmpty()) {
      }else if(0){ untested();
        // ?
        // BUG: use netlist format
        // BUG: parsing netlist.txt here??
        incomplete();

          /*! Only pass modules to Qucsator that are indeed used on
           * the schematic,it might be the case that the user loaded the icons,
           * but did not compiled the module. Qucsator will not find the library.
           *
           * Check if used symbols have corresponing lib before running
           * Qucsator? Need to search on the netlis.txt? Is there other data
           * structure containig the netlist?
           *
          */
          QStringList usedComponents;

          if (!NetlistFile.open(QIODevice::ReadOnly)) {
             QMessageBox::critical(this, tr("Error"), tr("Cannot read netlist!"));
          }else{
             QString net = QString(NetlistFile.readAll());

             QMapIterator<QString, QString> i(Module::vaComponents);
             while (i.hasNext()) {
                 i.next();
                 if (net.contains(i.key()))
                     usedComponents << i.key();
             }
             NetlistFile.close();
          }

          if (! usedComponents.isEmpty()) {
            /// \todo remvoe the command line arguments? use only netlist annotation?
            //Arguments << "-p" << QucsSettings.QucsWorkDir.absolutePath()
            //          << "-m" << usedComponents;
            //qDebug() << "taskElement :" << Program << Arguments.join(" ");

            /// Anotate netlist with Verilog-A dynamic path and module names
            ///
            if (!NetlistFile.open(QFile::Append | QFile::Text)){
               QMessageBox::critical(this, tr("Error"), tr("Cannot read netlist!"));
            }else{
               QTextStream out(&NetlistFile);
               out << "\n";
               out << "# --path=" << QucsSettings.QucsWorkDir.absolutePath() << "\n";
               out << "# --module=" << usedComponents.join(" ") << "\n";

               NetlistFile.close();
            }
          }
      } // vaComponents not empty

      {
        Program = QucsSettings.Qucsator;
        Arguments << "-b" << "-g" << "-i"
                  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                  << "-o" << DataSet;
      }
    } else {
      if (isVerilog) {
          Program = QDir::toNativeSeparators(QucsSettings.BinDir + QucsVeri);
          Arguments << QDir::toNativeSeparators(QucsSettings.QucsHomeDir.filePath("netlist.txt"))
                    << DataSet
                    << SimTime
                    << QDir::toNativeSeparators(SimPath)
                    << QDir::toNativeSeparators(QucsSettings.BinDir)
                    << "-c";
      } else {
/// \todo \bug error: unrecognized command line option '-Wl'
#ifdef __MINGW32__ // -> platform.h
    Program = QDir::toNativeSeparators(pathName(QucsSettings.BinDir + QucsDigi));
    Arguments << QDir::toNativeSeparators(QucsSettings.QucsHomeDir.filePath("netlist.txt"))
              << DataSet
              << SimTime
              << QDir::toNativeSeparators(SimPath)
              << QDir::toNativeSeparators(QucsSettings.BinDir) << "-Wall" << "-c";
#else
    Program = QDir::toNativeSeparators(pathName(QucsSettings.BinDir + QucsDigi));
    Arguments << QucsSettings.QucsHomeDir.filePath("netlist.txt")
              << DataSet << SimTime << pathName(SimPath)
		      << pathName(QucsSettings.BinDir) << "-Wall" << "-c";

#endif
      }
    }
  }else{
    incomplete();
//    throw ..
  }

  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()), SLOT(slotDisplayMsg()));
  connect(&SimProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                       SLOT(slotSimEnded(int, QProcess::ExitStatus)));
  connect(&SimProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
                       SLOT(slotStateChanged(QProcess::ProcessState)));

#ifdef SPEEDUP_PROGRESSBAR
  waitForUpdate = false;
#endif
  wasLF = false;

  ProgressText = "";

  QString sep(PATHSEP);
  SimProcess.setProcessEnvironment(env);

  qDebug() << "taskElement :" << Program << Arguments.join(" ");
  SimProcess.start(Program, Arguments); // launch the program

  delete sim;
}

// ------------------------------------------------------------------------
Component * SimMessage::findOptimization(SchematicDoc *)
{
#if 0
  for(auto pc : Doc->components()){
    if(pc->isActive){
      if(pc->obsolete_model_hack() == ".Opt"){
	return pc;
      }
    }
  }
#endif
  return NULL;
}


/*!
 * \brief called when the process sends an output to stdout.
 */
void SimMessage::slotDisplayMsg()
{
  int i;
  ProgressText += QString(SimProcess.readAllStandardOutput());
  if(wasLF) {
    i = ProgressText.lastIndexOf('\r');
    if(i > 1) {
#ifdef SPEEDUP_PROGRESSBAR
      iProgress = 10*int(ProgressText.at(i-2).toLatin1()-'0') +
                     int(ProgressText.at(i-1).toLatin1()-'0');
      if(!waitForUpdate) {
        QTimer::singleShot(20, this, SLOT(slotUpdateProgressBar()));
        waitForUpdate = true;
      }
#else
      SimProgress->setMaximum(100);
      SimProgress->setValue(
         10*int(ProgressText.at(i-2).toLatin1()-'0') +
            int(ProgressText.at(i-1).toLatin1()-'0'));
#endif
      ProgressText.remove(0, i+1);
    }

    if(ProgressText.size()>0&&ProgressText.at(0).toLatin1() <= '\t')
      return;
  }
  else {
    i = ProgressText.indexOf('\t'); // marker for progress indicator
    if(i >= 0) {
      wasLF = true;
      QString tmps = ProgressText.left(i).trimmed();
      if (!tmps.isEmpty()) // avoid adding a newline if no text to show
	ProgText->appendPlainText(tmps);
      ProgressText.remove(0, i+1);
      return;
    }
  }

  QString tmps = ProgressText.trimmed();
  if (!tmps.isEmpty()) // avoid adding a newline if no text to show
    ProgText->appendPlainText(tmps);
  ProgressText = "";
  wasLF = false;
}

#ifdef SPEEDUP_PROGRESSBAR
// ------------------------------------------------------------------------
void SimMessage::slotUpdateProgressBar()
{
  SimProgress->setProgress(iProgress, 100);
  waitForUpdate = false;
}
#endif

/*! 
 * \brief Insert process stderr output in the Error Message output window.
 *
 *  Called when the process sends an output to stderr.
 */
void SimMessage::slotDisplayErr()
{
  ErrText->appendPlainText(QString(SimProcess.readAllStandardError()));
}

/*!
 * \brief The process state changes.
 *
 *  Called when the process changes state;
 *  \param[in] newState new status of the process
 */
void SimMessage::slotStateChanged(QProcess::ProcessState newState)
{
  static QProcess::ProcessState oldState;
  qDebug() << "SimMessage::slotStateChanged() : newState = " << newState 
           << " " << SimProcess.error();
  switch(newState){
    case QProcess::NotRunning:
      switch(SimProcess.error()){
        case QProcess::FailedToStart: // does not happen (?)
        case QProcess::UnknownError: // getting here instead
          switch(oldState){
            case QProcess::Starting: // failed to start.
              ErrText->insertPlainText(tr("ERROR: Cannot start ") + Program +
                  " (" + SimProcess.errorString() + ")\n");
              FinishSimulation(-1);
              break;
            case QProcess::Running:
              // process ended without trouble.
              // slotSimEnded will be invoked soon.
              break;
            case QProcess::NotRunning:
              // impossible.
              break;
          }
          break;
        // note that on Windows negative exit codes are treated as 'crash'
        //   see comments in slotSimEnded() to handle this properly
        case QProcess::Crashed:
        case QProcess::Timedout:
        case QProcess::WriteError:
        case QProcess::ReadError:
          // nothing (yet)
          break;
      }
    break;
    case QProcess::Starting:
          ProgText->insertPlainText(tr("Starting ") + Program + "\n");
    break;
    case QProcess::Running:
    break;
  }
  oldState = newState;
}

/*!
 * \brief Check the simulation process exit status.
 *
 *  Called when the simulation process terminates; inserts an error
 *  message in the Error Message output window in case the simulation
 *  process does not exit normally.
 *  \param[in] exitCode exit code of the process
 *  \param[in] exitStatus exit status of the process
 *  \todo use a macro for the bugs report email (needs to define it for CMake)
 */
void SimMessage::slotSimEnded(int exitCode, QProcess::ExitStatus exitStatus )
{
  int stat = exitCode;

  if ((exitStatus != QProcess::NormalExit) &&
#ifdef _WIN32
// due to a bug in Qt, negative error codes are erroneously interpreted
//   as "program crashed", see https://bugreports.qt.io/browse/QTBUG-28735
// When we will switch to Qt5(.1) this code can be removed...
      (uint)stat >= 0x80000000U && (uint)stat < 0xD0000000U &&
#endif
      !simKilled) { // as when killed by user exitStatus will be QProcess::CrashExit
    stat = -1;
    ErrText->appendPlainText(tr("ERROR: Simulator crashed!"));
    ErrText->appendPlainText(tr("Please report this error to qucs-bugs@lists.sourceforge.net"));
  }
  FinishSimulation(stat); // 0 = normal , !=0 = error
}

/*!
 * \brief Add end-of-simulation messages and save the relevant data.
 *
 *  Called when the simulation ended with errors before starting the 
 *  simulator process.
 *  \param[in] Status exit status of the process (0 = normal, !=0 = error)
 */
void SimMessage::FinishSimulation(int Status)
{
  Abort->setText(tr("Close window"));
  Display->setDisabled(false);
  SimProgress->setValue(100);  // progress bar to 100%

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time

  if(Status == 0) {
    QString txt = tr("Simulation ended on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss:zzz"));
    ProgText->appendPlainText("\n" + txt + "\n" + tr("Ready."));
  }
  else {
    QString txt = tr("Errors occurred during simulation on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss:zzz"));
    ProgText->appendPlainText("\n" + txt + "\n" + tr("Aborted."));
  }

  QFile file(QucsSettings.QucsHomeDir.filePath("log.txt"));  // save simulator messages
  if(file.open(QIODevice::WriteOnly)) {
    QTextStream stream(&file);
    stream << tr("Output:\n-------") << "\n\n";
    for(int z=0; z<ProgText->document()->blockCount(); z++)
      stream << ProgText->document()->findBlockByNumber(z).text() << "\n";
    stream << "\n\n\n" << 
      tr("Errors and Warnings:\n--------------------") << "\n\n";
    for(int z=0; z<ErrText->document()->blockCount(); z++)
      stream << ErrText->document()->findBlockByNumber(z).text() << "\n";
    file.close();
  }

  if(Status == 0) {
    if(SimOpt) { // save optimization data
      QFile ifile(QucsSettings.QucsHomeDir.filePath("asco_out.dat"));
      QFile ofile(DataSet);
      if(ifile.open(QIODevice::ReadOnly)) {
	if(ofile.open(QIODevice::WriteOnly)) {
	  QByteArray data = ifile.readAll();
	  ofile.write(data);
	  ofile.close();
	}
	ifile.close();
      }
#if 0
      BUG.
      if(((Optimize_Sim*)SimOpt)->loadASCOout()){
	((SchematicDoc*)DocWidget)->setChanged(true,true);
      }
#endif
    }
  }

  emit SimulationEnded(Status, this);
}

/*!
 * \brief Close and delete simulation dialog
 *
 * To call accept(), which is protected, from the outside.
 * Called from the main GUI.
 */
void SimMessage::slotClose()
{
  accept();
}

// ------------------------------------------------------------------------
void SimMessage::slotDisplayButton()
{
  emit displayDataPage(DocName, DataDisplay);
  accept();
}

void SimMessage::AbortSim()
{
  ErrText->appendPlainText(tr("Simulation aborted by the user!"));
  simKilled = true;
  SimProcess.kill();
}
// vim:ts=8:sw=2:et
