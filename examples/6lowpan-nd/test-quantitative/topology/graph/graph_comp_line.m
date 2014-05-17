close all;
clear all;

%% data
%no_rpl_
%rpl_

no_rpl_number_of_note_start = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
no_rpl_time_start = [9189 14692 40518 87489 93615 108929 165080 222038 247476 273061 319548 336465 352117 367522 ];
no_rpl_msg_ip_start = [4, 1, 0, 0 ; 13, 3, 0, 0 ; 24, 7, 2, 0 ; 39, 13, 6, 0 ; 60, 21, 12, 0 ; 83, 31, 20, 0 ; 109, 43, 30, 0 ; 139, 57, 42, 0 ; 172, 73, 56, 0 ; 210, 91, 72, 0 ; 251, 111, 90, 0 ; 295, 133, 110, 0 ; 342, 157, 132, 0 ; 391, 183, 156, 0 ];
no_rpl_msg_nd6_start = [4, 4 ; 12, 12 ; 19, 18 ; 32, 28 ; 43, 38 ; 58, 48 ; 74, 59 ; 91, 71 ; 109, 84 ; 130, 98 ; 152, 112 ; 176, 127 ; 200, 142 ; 225, 157 ];

no_rpl_number_of_note_hour = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
no_rpl_time_hour = [3609216 3614733 3640577 3687562 3693703 3709033 3765201 3822160 3847612 3873197 3919715 3936632 3952299 3967718 ];
no_rpl_msg_ip_hour = [46, 16, 0, 0 ; 99, 34, 0, 0 ; 156, 70, 32, 0 ; 219, 110, 68, 0 ; 290, 154, 108, 0 ; 365, 202, 152, 0 ; 445, 254, 200, 0 ; 531, 310, 252, 0 ; 622, 370, 308, 0 ; 720, 434, 368, 0 ; 823, 502, 432, 0 ; 931, 574, 500, 0 ; 1044, 650, 572, 0 ; 1161, 730, 648, 0 ];
no_rpl_msg_nd6_hour = [46, 46 ; 98, 98 ; 121, 120 ; 150, 146 ; 177, 172 ; 208, 198 ; 240, 225 ; 273, 253 ; 307, 282 ; 344, 312 ; 382, 342 ; 422, 373 ; 462, 404 ; 503, 436 ];

rpl_number_of_note_start = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
rpl_time_start = [38784 121779 185854 206306 206140 309349 359320 413295 413398 518758 557323 563743 717045 703996 ];
rpl_msg_ip_start = [8, 1, 0, 0 ; 28, 3, 0, 0 ; 72, 13, 5, 0 ; 115, 19, 9, 0 ; 157, 27, 15, 0 ; 227, 37, 23, 0 ; 331, 61, 43, 0 ; 392, 63, 45, 0 ; 491, 79, 59, 0 ; 606, 93, 73, 0 ; 736, 122, 98, 0 ; 835, 144, 118, 0 ; 1042, 184, 155, 0 ; 1170, 210, 179, 0 ];
rpl_msg_nd6_start = [4, 4 ; 11, 11 ; 25, 25 ; 36, 35 ; 50, 46 ; 67, 58 ; 89, 76 ; 106, 85 ; 122, 98 ; 147, 111 ; 174, 133 ; 199, 150 ; 230, 171 ; 261, 190 ];

rpl_number_of_note_hour = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
rpl_time_hour = [3638789 3721786 3785864 3806319 3806155 3909367 3959342 4013320 4013424 4118789 4157356 4163779 4317084 4304038 ];
rpl_msg_ip_hour = [56, 16, 0, 0 ; 135, 34, 0, 0 ; 250, 76, 35, 0 ; 359, 116, 71, 0 ; 481, 160, 111, 0 ; 651, 208, 155, 0 ; 833, 272, 213, 0 ; 986, 316, 255, 0 ; 1176, 376, 311, 0 ; 1409, 436, 369, 0 ; 1689, 513, 440, 0 ; 1888, 585, 508, 0 ; 2190, 677, 595, 0 ; 2475, 757, 671, 0 ];
rpl_msg_nd6_hour = [46, 46 ; 97, 97 ; 127, 127 ; 154, 153 ; 184, 180 ; 217, 208 ; 255, 242 ; 288, 267 ; 320, 296 ; 367, 331 ; 410, 369 ; 451, 402 ; 504, 445 ; 551, 480 ];


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