% data
x = {'6lh'; '6lr'; '6lbr'; 'rpl'; '6lr-rpl'; '6lbr-rpl'};
y = [ 31710 168 5992 ; 36346 168 6148 ; 31972 168 6720 ; 43883 192 6182 ; 48754 192 6552 ; 44490 192 7124 ];
% enddata

% Construct graph
figure;
bar(y,'stacked');
set(gca,'XTickLabels',x); 
legend('Code','Initialized variables','Uninitialized variables',2);
xlabel('Entity');
ylabel('memory (bytes)');
title('msp430-size over 6LoWPAN-ND entities');