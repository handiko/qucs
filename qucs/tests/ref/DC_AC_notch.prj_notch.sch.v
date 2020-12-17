module main();
GND #() *(gnd);
C #(.C(1.01u), .V(), .Symbol(neutral)) C1(_net9, _net15);
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R5(In, Out);
R #(.R(1k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(_net10, _net18);
R #(.R(1k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R4(_net5, _net18);
R #(.R(1k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(_net15, _net10);
C #(.C(1u), .V(), .Symbol(neutral)) C2(_net5, Out);
OpAmp #(.G(1e6), .Umax(15 V)) OP1(_net5, _net9, _net15);
OpAmp #(.G(1e6), .Umax(15 V)) OP2(_net5, _net10, _net18);
Eqn #(.Gain(dB(Out.v/In.v)), .Export(yes)) Eqn1();
R #(.R(10k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(gnd, _net9);
GND #() *(gnd);
Vac #(.U(1uV), .f(1 GHz), .Phase(0), .Theta(0)) V1(In, gnd);
endmodule // main

Sub #(.File()) Sub(_net0);
