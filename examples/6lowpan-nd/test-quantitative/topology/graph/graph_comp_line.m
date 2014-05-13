close all;
clear all;

%% data
%no_rpl_
%rpl_

no_rpl_number_of_note_start = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
no_rpl_time_start = [24190 29816 55520 102366 149242 194052 240207 257165 302353 348189 394550 411343 457744 502646 ];
no_rpl_msg_ip_start = [4, 1, 0, 0 ; 13, 3, 0, 0 ; 24, 7, 2, 0 ; 39, 13, 6, 0 ; 60, 21, 12, 0 ; 84, 31, 20, 0 ; 112, 43, 30, 0 ; 143, 57, 42, 0 ; 177, 73, 56, 0 ; 215, 91, 72, 0 ; 257, 111, 90, 0 ; 302, 133, 110, 0 ; 350, 157, 132, 0 ; 401, 183, 156, 0 ];
no_rpl_msg_nd6_start = [4, 4 ; 12, 12 ; 19, 18 ; 32, 28 ; 43, 38 ; 59, 49 ; 76, 61 ; 94, 73 ; 114, 86 ; 135, 100 ; 158, 115 ; 182, 130 ; 208, 146 ; 234, 162 ];

no_rpl_number_of_note_hour = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
no_rpl_time_hour = [3624219 3629861 3655580 3702440 3749332 3794161 3840329 3857303 3902490 3948342 3994734 4011526 4057943 4102864 ];
no_rpl_msg_ip_hour = [54, 16, 0, 0 ; 107, 34, 0, 0 ; 164, 70, 32, 0 ; 227, 110, 68, 0 ; 298, 154, 108, 0 ; 374, 202, 152, 0 ; 456, 254, 200, 0 ; 543, 310, 252, 0 ; 635, 370, 308, 0 ; 731, 434, 368, 0 ; 835, 502, 432, 0 ; 944, 574, 500, 0 ; 1062, 650, 572, 0 ; 1189, 730, 648, 0 ];
no_rpl_msg_nd6_hour = [54, 54 ; 106, 106 ; 129, 128 ; 158, 154 ; 185, 180 ; 217, 207 ; 250, 235 ; 284, 263 ; 320, 292 ; 355, 320 ; 394, 351 ; 434, 382 ; 480, 418 ; 530, 458 ];

rpl_number_of_note_start = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
rpl_time_start = [38796 126900 210098 310929 319760 320724 438690 471666 537516 566629 661365 725188 714984 850366 ];
rpl_msg_ip_start = [8, 1, 0, 0 ; 28, 3, 0, 0 ; 71, 13, 5, 0 ; 137, 19, 9, 0 ; 197, 27, 15, 0 ; 246, 37, 23, 0 ; 317, 49, 33, 0 ; 417, 63, 45, 0 ; 525, 79, 59, 0 ; 590, 93, 73, 0 ; 720, 125, 100, 0 ; 868, 147, 120, 0 ; 994, 171, 142, 0 ; 1192, 197, 166, 0 ];
rpl_msg_nd6_start = [4, 4 ; 11, 11 ; 25, 25 ; 36, 35 ; 51, 46 ; 68, 58 ; 85, 71 ; 108, 85 ; 123, 100 ; 144, 113 ; 167, 129 ; 194, 146 ; 223, 164 ; 256, 185 ];

rpl_number_of_note_hour = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
rpl_time_hour = [3638801 3726907 3810108 3910941 3919775 3920742 4038711 4071690 4137543 4166658 4261399 4325224 4315023 4450408 ];
rpl_msg_ip_hour = [62, 16, 0, 0 ; 144, 34, 0, 0 ; 258, 76, 35, 0 ; 396, 116, 71, 0 ; 539, 160, 111, 0 ; 647, 208, 155, 0 ; 846, 260, 203, 0 ; 1021, 316, 255, 0 ; 1218, 376, 311, 0 ; 1381, 436, 369, 0 ; 1575, 516, 442, 0 ; 1886, 588, 510, 0 ; 2171, 664, 582, 0 ; 2484, 744, 658, 0 ];
rpl_msg_nd6_hour = [54, 54 ; 105, 105 ; 137, 137 ; 164, 163 ; 195, 190 ; 226, 216 ; 259, 245 ; 296, 273 ; 333, 310 ; 368, 337 ; 413, 375 ; 464, 416 ; 509, 450 ; 560, 489 ];


% enddata

no_rpl_msg_ip_dif = no_rpl_msg_ip_hour - no_rpl_msg_ip_start;
no_rpl_msg_nd6_dif = no_rpl_msg_nd6_hour - no_rpl_msg_nd6_start;

rpl_msg_ip_dif = rpl_msg_ip_hour - rpl_msg_ip_start;
rpl_msg_nd6_dif = rpl_msg_nd6_hour - rpl_msg_nd6_start;

number_of_note = rpl_number_of_note_start;

l = min(length(no_rpl_number_of_note_start),length(rpl_number_of_note_start));

%% Graphe

% Time
figure;
plot(number_of_note(1:l), [no_rpl_time_start(1:l)', rpl_time_start(1:l)']./1000 ,'*-');
legend('no RPL','RPL',2);
ylabel('Convergence time (s)');
xlabel('Number of nodes');
title('Convergence time of the network (Nexthop)');

% Msg ND6
figure;
plot(number_of_note, [no_rpl_msg_nd6_start rpl_msg_nd6_start],'*-');
legend('recv - no RPL','sent - no RPL','recv - RPL','sent - RPL',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (Nexthop) - starting');

% Msg ND6 - 1hour
figure;
plot(number_of_note, [no_rpl_msg_nd6_dif rpl_msg_nd6_dif],'*-');
legend('recv - no RPL','sent - no RPL','recv - RPL','sent - RPL',2);
ylabel('Number of messages');
xlabel('Number of nodes');
title('Number of ND6 messages (Nexthop) - 1 hour');

% IP packet rcv
figure;
no_rpl = no_rpl_msg_ip_dif(:,1);
rpl = rpl_msg_ip_dif(:,1);
plot(number_of_note(1:l), [no_rpl(1:l), rpl(1:l)] ,'*-');
legend('no RPL','RPL',2);
ylabel('Number of messages received');
xlabel('Number of nodes');
title('Number of IP messages received (Nexthop) - 1 hour');