module main();
GND #() anonymous_gnd_hack_0(net_650_120);
Vdc #(.U(15 V)) V1(net_650_60, net_650_120);
IProbe #() Collector(net_470_140, net_470_200);
R #(.R(20 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_470_120, net_470_60);
Vac #(.U(1 V), .f(1 GHz), .Phase(0), .Theta(0)) V2(net_340_120, net_400_120);
Diode #(.Is(1e-15 A), .N(1), .Cj0(10 pF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal)) D2(net_170_90, net_170_30);
Diode #(.Is(1e-15 A), .N(1), .Cj0(10 pF), .M(0.5), .Vj(0.7 V), .Fc(0.5), .Cp(0.0 fF), .Isr(0.0), .Nr(2.0), .Rs(0.0 Ohm), .Tt(0.0 ps), .Ikf(0), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Bv(0), .Ibv(1 mA), .Temp(26.85), .Xti(3.0), .Eg(1.11), .Tbv(0.0), .Trs(0.0), .Ttt1(0.0), .Ttt2(0.0), .Tm1(0.0), .Tm2(0.0), .Tnom(26.85), .Area(1.0), .Symbol(normal)) D1(net_170_150, net_170_90);
_BJT #(.Type(pnp), .Is(1e-16), .Nf(1), .Nr(1), .Ikf(0), .Ikr(0), .Vaf(0), .Var(0), .Ise(0), .Ne(1.5), .Isc(0), .Nc(2), .Bf(300), .Br(1), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(30 pF), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T2(net_190_150, net_220_180, net_220_120, net_220_180);
GND #() anonymous_gnd_hack_1(net_50_460);
C #(.C(1 nF), .V(), .Symbol(neutral)) C4(net_160_400, net_220_400);
GND #() anonymous_gnd_hack_2(net_470_460);
GND #() anonymous_gnd_hack_3(net_650_460);
GND #() anonymous_gnd_hack_4(net_610_270);
C #(.C(1 nF), .V(), .Symbol(neutral)) C3(net_550_360, net_610_360);
L #(.L(100 nH), .I()) L1(net_470_350, net_470_290);
Pac #(.Num(1), .Z(50 Ohm), .P(0), .f(1 GHz), .Temp(26.85)) P1(net_50_400, net_50_460);
Pac #(.Num(2), .Z(50 Ohm), .P(0), .f(1 GHz), .Temp(26.85)) P2(net_650_400, net_650_460);
_BJT #(.Type(npn), .Is(1e-16), .Nf(1), .Nr(1), .Ikf(0), .Ikr(0), .Vaf(0), .Var(0), .Ise(0), .Ne(1.5), .Isc(0), .Nc(2), .Bf(150), .Br(1), .Rbm(0), .Irb(0), .Rc(0), .Re(0), .Rb(0), .Cje(10 pF), .Vje(0.75), .Mje(0.33), .Cjc(0), .Vjc(0.75), .Mjc(0.33), .Xcjc(1.0), .Cjs(0), .Vjs(0.75), .Mjs(0), .Fc(0.5), .Tf(0.0), .Xtf(0.0), .Vtf(0.0), .Itf(0.0), .Tr(0.0), .Temp(26.85), .Kf(0.0), .Af(1.0), .Ffe(1.0), .Kb(0.0), .Ab(1.0), .Fb(1.0), .Ptf(0.0), .Xtb(0.0), .Xti(3.0), .Eg(1.11), .Tnom(26.85), .Area(1.0)) T1(net_440_400, net_470_370, net_470_430, net_470_370);
IProbe #() Basis(net_380_400, net_440_400);
L #(.L(100 nH), .I()) L2(net_220_400, net_220_340);
GND #() anonymous_gnd_hack_5(net_360_270);
R #(.R(10 Ohm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(net_220_340, net_220_280);
GND #() anonymous_gnd_hack_6(net_170_210);
R #(.R(20 kOhm), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_170_210, net_170_150);
Eqn #(.LoopGain(output.v / input.v), .LoopPhase(phase(output.v / input.v)), .Export(yes)) Eqn1();
C #(.C(0.1 uF), .V(), .Symbol(neutral)) C1(net_550_270, net_610_270);
C #(.C(5 uF), .V(), .Symbol(neutral)) C2(net_300_270, net_360_270);
wire #() noname(net_170_150, net_190_150);
wire #() noname(net_470_120, net_470_140);
wire #() noname(net_470_30, net_470_60);
wire #() noname(net_470_30, net_650_30);
wire #() noname(net_650_30, net_650_60);
wire #() noname(net_170_30, net_470_30);
wire #() noname(net_220_120, net_340_120);
wire #() noname(net_400_120, net_470_120);
wire #() noname(net_50_400, net_160_400);
wire #() noname(net_470_430, net_470_460);
wire #() noname(net_470_350, net_470_360);
wire #() noname(net_470_360, net_470_370);
wire #() noname(net_470_360, net_550_360);
wire #() noname(net_650_360, net_650_400);
wire #() noname(net_610_360, net_650_360);
wire #() noname(net_470_270, net_470_290);
wire #() noname(net_470_270, net_550_270);
wire #() noname(net_220_400, net_380_400);
wire #() noname(net_220_180, net_220_270);
wire #() noname(net_470_200, net_470_270);
wire #() noname(net_220_270, net_220_280);
wire #() noname(net_220_270, net_300_270);
place #(.$xposition(50),.$yposition(400)) place_50_400(net_50_400);
place #(.$xposition(50),.$yposition(460)) place_50_460(net_50_460);
place #(.$xposition(160),.$yposition(400)) place_160_400(net_160_400);
place #(.$xposition(170),.$yposition(30)) place_170_30(net_170_30);
place #(.$xposition(170),.$yposition(90)) place_170_90(net_170_90);
place #(.$xposition(170),.$yposition(150)) place_170_150(net_170_150);
place #(.$xposition(170),.$yposition(210)) place_170_210(net_170_210);
place #(.$xposition(190),.$yposition(150)) place_190_150(net_190_150);
place #(.$xposition(220),.$yposition(120)) place_220_120(net_220_120);
place #(.$xposition(220),.$yposition(180)) place_220_180(net_220_180);
place #(.$xposition(220),.$yposition(270)) place_220_270(net_220_270);
place #(.$xposition(220),.$yposition(280)) place_220_280(net_220_280);
place #(.$xposition(220),.$yposition(340)) place_220_340(net_220_340);
place #(.$xposition(220),.$yposition(400)) place_220_400(net_220_400);
place #(.$xposition(300),.$yposition(270)) place_300_270(net_300_270);
place #(.$xposition(340),.$yposition(120)) place_340_120(net_340_120);
place #(.$xposition(360),.$yposition(270)) place_360_270(net_360_270);
place #(.$xposition(380),.$yposition(400)) place_380_400(net_380_400);
place #(.$xposition(400),.$yposition(120)) place_400_120(net_400_120);
place #(.$xposition(440),.$yposition(400)) place_440_400(net_440_400);
place #(.$xposition(470),.$yposition(30)) place_470_30(net_470_30);
place #(.$xposition(470),.$yposition(60)) place_470_60(net_470_60);
place #(.$xposition(470),.$yposition(120)) place_470_120(net_470_120);
place #(.$xposition(470),.$yposition(140)) place_470_140(net_470_140);
place #(.$xposition(470),.$yposition(200)) place_470_200(net_470_200);
place #(.$xposition(470),.$yposition(270)) place_470_270(net_470_270);
place #(.$xposition(470),.$yposition(290)) place_470_290(net_470_290);
place #(.$xposition(470),.$yposition(350)) place_470_350(net_470_350);
place #(.$xposition(470),.$yposition(360)) place_470_360(net_470_360);
place #(.$xposition(470),.$yposition(370)) place_470_370(net_470_370);
place #(.$xposition(470),.$yposition(430)) place_470_430(net_470_430);
place #(.$xposition(470),.$yposition(460)) place_470_460(net_470_460);
place #(.$xposition(550),.$yposition(270)) place_550_270(net_550_270);
place #(.$xposition(550),.$yposition(360)) place_550_360(net_550_360);
place #(.$xposition(610),.$yposition(270)) place_610_270(net_610_270);
place #(.$xposition(610),.$yposition(360)) place_610_360(net_610_360);
place #(.$xposition(650),.$yposition(30)) place_650_30(net_650_30);
place #(.$xposition(650),.$yposition(60)) place_650_60(net_650_60);
place #(.$xposition(650),.$yposition(120)) place_650_120(net_650_120);
place #(.$xposition(650),.$yposition(360)) place_650_360(net_650_360);
place #(.$xposition(650),.$yposition(400)) place_650_400(net_650_400);
place #(.$xposition(650),.$yposition(460)) place_650_460(net_650_460);
endmodule // main

Sub #(.File()) Sub(net_0_0);
// skip sckt :SymbolSection:
// skip sckt :Diagrams:
// skip sckt :Paintings:
