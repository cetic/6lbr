
%% data
%no_rpl_
%rpl_

no_rpl_number_of_note_start = [2 3 4 5 6 7 8 9 10 11 12 13 14 15 ];
rpl_number_of_note_start = [2 3 4 5 6 ];

no_rpl_time_start = [38935 210975 209853 370111 535475 ];
rpl_time_start = [24190 29692 55520 102366 149242 194052 240082 257165 302353 348314 394550 411467 457494 502646 ];

rpl_msg_ip_hour = [53, 16, 0, 0 ; 141, 34, 0, 0 ; 244, 72, 33, 0 ; 395, 122, 75, 0 ; 554, 168, 117, 0 ];
no_rpl_msg_ip_hour = [46, 16, 0, 0 ; 99, 34, 0, 0 ; 156, 70, 32, 0 ; 219, 110, 68, 0 ; 290, 154, 108, 0 ; 366, 202, 152, 0 ; 448, 254, 200, 0 ; 535, 310, 252, 0 ; 627, 370, 308, 0 ; 725, 434, 368, 0 ; 829, 502, 432, 0 ; 938, 574, 500, 0 ; 1056, 650, 572, 0 ; 1177, 730, 648, 0 ];

% enddata

l = min(length(no_rpl_number_of_note_start),length(rpl_number_of_note_start));

%% Graphe

% Time
figure;
plot(number_of_note(1:l), [no_rpl_time_start(1:l)', rpl_time_start(1:l)']./1000 ,'*-');
legend('no RPL','RPL',2);
ylabel('Convergence time (s)');
xlabel('Number of nodes');
title('Convergence time of the network');

% IP packet rcv
figure;
no_rpl_rcv = no_rpl_msg_ip_hour(:,1);
rpl_rcv = rpl_msg_ip_hour(:,1);
plot(number_of_note(1:l), [no_rpl_rcv(1:l), rpl_rcv(1:l)] ,'*-');
legend('no RPL','RPL',2);
ylabel('Number of messages received');
xlabel('Number of nodes');
title('Number of IP messages received - 1 hour');