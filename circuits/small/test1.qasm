OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
creg c[5];
cx q[2],q[3];
h q[0];
t q[4];
cx q[4],q[1];
cx q[0],q[4];