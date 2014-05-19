close all;
clear all;

%% data
%ld_
%hd_

ld_number_of_note_start = [4 9 16 25 36 49 64 ];
ld_time_start = [226486 361608 348336 462640 598052 708886 920893 ];
ld_msg_ip_start = [47, 6, 0, 0 ; 378, 48, 24, 0 ; 900, 132, 82, 0 ; 1826, 272, 196, 0 ;3355, 506, 402, 0 ; 5318, 924, 756, 0 ; 7915, 1308, 1108, 0 ];
ld_msg_nd6_start = [28, 32 ; 150, 112 ; 228, 185 ; 442, 306 ;752, 505 ; 1167, 778 ; 1591, 1015 ];

ld_number_of_note_hour = [4 9 16 25 36 49 64 ];
ld_time_hour = [3826496 3961632 3948380 4062709 4198154 4309025 4521078 ];
ld_msg_ip_hour = [246, 68, 0, 0 ; 810, 156, 88, 0 ; 1948, 510, 368, 0 ; 3650, 882, 690, 0 ;6138, 1404, 1164, 0 ; 8959, 1810, 1506, 0 ; 13775, 2928, 2532, 0 ];
ld_msg_nd6_hour = [196, 205 ; 342, 309 ; 624, 601 ; 1032, 935 ;1513, 1300 ; 2143, 1796 ; 2923, 2398 ];

hd_number_of_note_start = [4 9 16 25 36 49 64 ];
hd_time_start = [80983 123444 222965 300428 485101 494014 1210327 ];
hd_msg_ip_start = [62, 5, 0, 0 ; 284, 15, 0, 0 ; 1068, 101, 49, 0 ; 2456, 383, 285, 0;4417, 413, 292, 0 ; 6432, 796, 605, 0 ; 14295, 4092, 3824, 8  ];
hd_msg_nd6_start = [25, 20 ; 84, 52 ; 263, 196 ; 574, 353 ;992, 499 ; 1437, 764 ; 2337, 1273 ];

hd_number_of_note_hour = [4 9 16 25 36 49 64 ];
hd_time_hour = [3680992 3723466 3823008 3900497 4085202 4094155 4810514 ];
hd_msg_ip_hour = [262, 52, 0, 0 ; 735, 58, 0, 0 ; 2031, 292, 153, 0 ; 4262, 788, 575, 0 ;6912, 808, 576, 0 ; 10238, 1444, 1109, 0 ; 45190, 9539, 9039, 23 ];
hd_msg_nd6_hour = [163, 163 ; 274, 247 ; 633, 576 ; 1141, 952 ; 1752, 1319 ; 2433, 1827 ; 3906, 2971 ];

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