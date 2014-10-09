close all;

%% data
number_of_note_start = [2 3 4 5 6 ];
time_start = [38935 210975 209853 370111 535475 ];
msg_ip_start = [9, 1, 0, 0 ; 35, 3, 0, 0 ; 67, 9, 3, 0 ; 146, 25, 13, 0 ; 226, 35, 21, 0 ];
msg_nd6_start = [6, 6 ; 13, 13 ; 24, 24 ; 41, 39 ; 55, 50 ];

number_of_note_hour = [2 3 4 5 6 ];
time_hour = [3638939 3810982 3809862 3970123 4135491 ];
msg_ip_hour = [53, 16, 0, 0 ; 141, 34, 0, 0 ; 244, 72, 33, 0 ; 395, 122, 75, 0 ; 554, 168, 117, 0 ];
msg_nd6_hour = [48, 48 ; 99, 99 ; 126, 126 ; 159, 157 ; 195, 190 ];
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
