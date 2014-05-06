close all;

%% data
number_of_note_start = [2 4 6 8 10 ];
time_start = [24189 55518 148615 240081 302352 ];
msg_ip_start = [4, 1, 0, 0 ; 24, 7, 2, 0 ; 60, 21, 12, 0 ; 112, 43, 30, 0 ; 176, 73, 56, 0 ];
msg_nd6_start = [4, 4 ; 19, 18 ; 43, 38 ; 76, 61 ; 113, 86 ];

number_of_note_hour = [2 4 6 8 10 ];
time_hour = [3624216 3655577 3748703 3840201 3902487 ];
msg_ip_hour = [46, 16, 0, 0 ; 156, 70, 32, 0 ; 290, 154, 108, 0 ; 448, 254, 200, 0 ; 626, 370, 308, 0 ];
msg_nd6_hour = [46, 46 ; 121, 120 ; 177, 172 ; 242, 227 ; 311, 284 ];
% enddata

number_of_note = number_of_note_start;

time_dif = time_hour-time_start;
msg_ip_dif = msg_ip_hour-msg_ip_start;
msg_nd6_dif = msg_nd6_hour-msg_nd6_start;


%% Graph Starting
% Timing
figure;
plot(number_of_note, time_start./1000,'*-');
ylabel('Convergence time (s)');
xlabel('Number of nodes');
title('Convergence time of the network (line topology) - starting');

% Msg IP
figure;
plot(number_of_note, msg_ip_start,'*-');
legend('recv','sent','forwarded', 'drop',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of IP messages (line topology) - starting');

% Msg ND6
figure;
plot(number_of_note, msg_nd6_start,'*-');
legend('recv','sent',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (line topology) - starting');


%% Graph 1hour
% Timing
% figure;
% plot(number_of_note, time_dif,'*-');
% ylabel('Convergence time (ms)');
% xlabel('Number of nodes');
% title('Convergence time of the network (line topology) - 1 hour');

% Msg IP
figure;
plot(number_of_note, msg_ip_dif,'*-');
legend('recv','sent','forwarded', 'drop',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of IP messages (line topology) - 1 hour');

% Msg ND6
figure;
plot(number_of_note, msg_nd6_dif,'*-');
legend('recv','sent',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (line topology) - 1 hour');

%% Graph All
% Timing
% figure;
% plot(number_of_note, time_hour,'*-');
% ylabel('Convergence time (ms)');
% xlabel('Number of nodes');
% title('Convergence time of the network (line topology) - All');

% Msg IP
figure;
plot(number_of_note, msg_ip_hour,'*-');
legend('recv','sent','forwarded', 'drop',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of IP messages (line topology) - All');

% Msg ND6
figure;
plot(number_of_note, msg_nd6_hour,'*-');
legend('recv','sent',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (line topology) - All');
