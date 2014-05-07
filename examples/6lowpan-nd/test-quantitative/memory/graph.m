% data
x = {'6lh'; '6lr'; '6lbr'; 'rpl'; '6lr-rpl'; '6lbr-rpl'};
y = [ 31848 168 5992 ; 36534 168 6148 ; 32150 168 6720 ; 43889 192 6182 ; 48942 192 6552 ; 44668 192 7124 ];
% enddata

% Construct graph
figure;
bar(y,'stacked');
set(gca,'XTickLabels',x); 
legend('Code','Initialized variables','Uninitialized variables',2);
xlabel('Entity');
ylabel('memory (bytes)');
title('msp430-size over 6LoWPAN-ND entities');