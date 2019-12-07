% THIS SCRIPT CALCULATES THEORETICAL VALUES FOR THE PID CONTROLER %

%%%%%%%%%%%%%%% Bike physical parameters %%%%%%%%%%%%%%%%%%

m_b = 0.3046; %Bike mass 

m_w = 0.2804; %Motor + Flywheel mass

I_b = 0.043; %Bike moment of inertia

I_w = 1.5E-5; %Flywheel + Motor moment of inertia

L_b = 0.292; %Bike height (to the center of the flywheel)

L_c = 0.258; %Height of bike COM

g = 9.8; %Gravitationnal constant

%%%%%%%%%%%%%%%% Motor parameters %%%%%%%%%%%%%%%%%

max_voltage = 12; %Max voltage in volts

stall_torque = 0.297; %In Nm

stall_current = 2.1; %In amps


%%%%%%%%%%%%%%% Sensors gains %%%%%%%%%%%%%%%%%%%%%

Ka = 12; %Conversion between PWM and voltage

Km = 1; %Position sensor gain

%%%%%%%%%%%%%%% Motor constants %%%%%%%%%%%%%%%%%%%

M = stall_torque/stall_current; %Torque constant, Nm/A

Ra = max_voltage/stall_current; %Armature resistance, in Ohms

M = 0.0375;
Ra = 4.7;
Ka = 1.168;
Km = 107.47;

%%%%%%%%%%%%%%% Equations constants %%%%%%%%%%%%%%%%

a_1 = m_w * L_b^2 + I_b;

a_2 = (m_b*L_c + m_w*L_b)*g;

gamma = M/Ra * (1/I_w + 1/a_1);

beta = M*Ka/(a_1 * Ra);

delta = M*M/(a_1*Ra);

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

%PID control
Kp = 16.1;
Ki = 61.92;
Kd = 0.644;

pid_tf = tf(pid(Kp,Ki,Kd));


%Transfer function of closed loop system:

T = tf(feedback(transfer*pid_tf,1));
[a,b,c,d] = (tf2ss(cell2mat(T.Num), cell2mat(T.Den)));
closed_loop_system = ss(a,b,c,d);
poles = roots(cell2mat(T.Den));
zeros = roots(cell2mat(T.Num));

%Response to intial condition:
theta_0 = 0.02; %radians
t = 0:1E-3:10;
u = 0*t;
theta_desired = 0*t;
tFinal = 1;
%lsim(closed_loop_system, theta_desired, t, [theta_0 0 0 0]);

rltool(closed_loop_system);


