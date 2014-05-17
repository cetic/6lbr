close all;
clear all;

%% data
%ld_
%hd_

ld_number_of_note_start = [4 9 16 25 ];
ld_time_start = [226486 361608 348336 462640 ];
ld_msg_ip_start = [47, 6, 0, 0 ; 378, 48, 24, 0 ; 900, 132, 82, 0 ; 1826, 272, 196, 0 ];
ld_msg_nd6_start = [28, 32 ; 150, 112 ; 228, 185 ; 442, 306 ];

ld_number_of_note_hour = [4 9 16 25 ];
ld_time_hour = [3826496 3961632 3948380 4062709 ];
ld_msg_ip_hour = [246, 68, 0, 0 ; 810, 156, 88, 0 ; 1948, 510, 368, 0 ; 3650, 882, 690, 0 ];
ld_msg_nd6_hour = [196, 205 ; 342, 309 ; 624, 601 ; 1032, 935 ];

hd_number_of_note_start = [4 9 16 25 ];
hd_time_start = [80983 123444 222965 300428 ];
hd_msg_ip_start = [62, 5, 0, 0 ; 284, 15, 0, 0 ; 1068, 101, 49, 0 ; 2456, 383, 285, 0 ];
hd_msg_nd6_start = [25, 20 ; 84, 52 ; 263, 196 ; 574, 353 ];

hd_number_of_note_hour = [4 9 16 25 ];
hd_time_hour = [3680992 3723466 3823008 3900497 ];
hd_msg_ip_hour = [262, 52, 0, 0 ; 735, 58, 0, 0 ; 2031, 292, 153, 0 ; 4262, 788, 575, 0 ];
hd_msg_nd6_hour = [163, 163 ; 274, 247 ; 633, 576 ; 1141, 952 ];


% enddata

ld_msg_ip_dif = ld_msg_ip_hour - ld_msg_ip_start;
ld_msg_nd6_dif = ld_msg_nd6_hour - ld_msg_nd6_start;

hd_msg_ip_dif = hd_msg_ip_hour - hd_msg_ip_start;
hd_msg_nd6_dif = hd_msg_nd6_hour - hd_msg_nd6_start;

number_of_note = hd_number_of_note_start;

l = min(length(ld_number_of_note_start),length(hd_number_of_note_start));

%% Graphe

% Time
figure;
plot(number_of_note(1:l), [ld_time_start(1:l)', hd_time_start(1:l)']./1000 ,'*-');
legend('low density','high density',2);
ylabel('Convergence time (s)');
xlabel('Number of nodes');
title('Convergence time of the network (Grid)');

% Msg ND6
figure;
plot(number_of_note, [ld_msg_nd6_start hd_msg_nd6_start],'*-');
legend('recv - low density','sent - low density','recv - high density','sent - high density',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (Grid) - starting');

% Msg ND6 - 1hour
figure;
plot(number_of_note, [ld_msg_nd6_dif hd_msg_nd6_dif],'*-');
legend('recv - low density','sent - low density','recv - high density','sent - high density',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (Grid) - 1 hour');

% IP packet rcv
figure;
a = ld_msg_ip_dif(:,1);
b = hd_msg_ip_dif(:,1);
plot(number_of_note(1:l), [a(1:l), b(1:l)] ,'*-');
legend('low density','high density',2);
ylabel('Number of messages received');
xlabel('Number of nodes');
title('Number of IP messages received (Grid) - 1 hour');