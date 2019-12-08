% THIS SCRIPT CALCULATES THEORETICAL VALUES FOR THE PID CONTROLER %

%% Bike Physical Parameters
m_b = 0.3046; %Bike mass 

m_w = 0.2804; %Motor + Flywheel mass

I_b = 0.043; %Bike moment of inertia

I_w = 1.5E-5; %Flywheel + Motor moment of inertia

L_b = 0.292; %Bike height (to the center of the flywheel)

L_c = 0.258; %Height of bike COM

g = 9.8; %Gravitationnal constant

%% Motor Parameters
max_voltage = 12; %Max voltage in volts

stall_torque = 0.297; %In Nm

stall_current = 2.1; %In amps

%% Sensor Gains
Ka = 12; %Conversion between PWM and voltage

Km = 1; %Position sensor gain

%% Motor Constants
M = stall_torque/stall_current; %Torque constant, Nm/A

Ra = max_voltage/stall_current; %Armature resistance, in Ohms

M = 0.0375;
Ra = 4.7;
Ka = 1.168;
Km = 107.47;

%% Equation Constants
a_1 = m_w * L_b^2 + I_b;

a_2 = (m_b*L_c + m_w*L_b)*g;

gamma = M/Ra * (1/I_w + 1/a_1);

beta = M*Ka/(a_1 * Ra);

delta = M*M/(a_1*Ra);

%% Defining Open Loop System 
%%%%%%%%%%%%%% Defining simulink's transfer function %%%%%%%%%%%
%System matrix
A = [[0 1 0]; [a_2/a_1 0 delta]; [-a_2/a_1 0 -M*gamma]];
B = transpose([0 beta -Ka*gamma]);
C = [Km 0 0];
D = 0;
open_loop_sys = ss(A,B,C,D);

%Transfer function
[num,denum] = ss2tf(A,B,C,D);
transfer = tf(num, denum);

%% PID control

Tpaper = tf([9.6951, 93.9843, 227.771],[1, 14.541, 70.4802, 113.873]);
C = -Tpaper/(transfer*(Tpaper-1));

Kp = 16.1;
Ki = 61.92;
Kd = 0.644;
pid_tf = tf(pid(Kp,Ki,Kd));


%Transfer function of closed loop system:
% Poles and zeros of paper T for comparison
[a,b,c,d] = (tf2ss(cell2mat(Tpaper.Num), cell2mat(Tpaper.Den)));
closed_loop_system = ss(a,b,c,d);
poles_paper = roots(cell2mat(Tpaper.Den));
zeros_paper = roots(cell2mat(Tpaper.Num));

T = tf(feedback(transfer*pid_tf,1));
[a,b,c,d] = (tf2ss(cell2mat(T.Num), cell2mat(T.Den)));
closed_loop_system = ss(a,b,c,d);
poles = roots(cell2mat(T.Den));
zero = roots(cell2mat(T.Num));

%% Internally Stable Design - Section 4 of paper
sensitivity_u = pid_tf/(1+pid_tf*transfer);
poles = roots(cell2mat(sensitivity_u.Den));
zero = roots(cell2mat(sensitivity_u.Num));

Kf = -1.24;
af = 19.95;
F = tf(Kf,[1,af]);

H = F*pid_tf*transfer/(1+pid_tf*(F+transfer));
[a,b,c,d] = (tf2ss(cell2mat(H.Num), cell2mat(H.Den)));
H_ss = ss(a,b,c,d);

[a,b,c,d] = tf2ss([-4.1158, 19.9534],[1 19.95]);
H_ss_paper= ss(a,b,c,d);

%% Response to intial condition:
theta_0 = -0.2; %radians
t = 0:1E-2:8;
u = 0*t;
theta_desired = 0*t;
tFinal = 1;
lsim(H_ss, theta_desired, t, [theta_0, zeros(1,9)]);

%rltool(closed_loop_system);


