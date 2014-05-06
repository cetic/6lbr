% data
x = {'6lh'; '6lr'; '6lbr'};
y = [ 37277 184 7020 ; 41295 184 7176 ; 37817 184 7748 ];
% enddata

% Construct graph
figure;
bar(y,'stacked');
set(gca,'XTickLabels',x); 
legend('Code','Initialized variables','Uninitialized variables',2);
xlabel('Entity');
ylabel('memory (bytes)');
title('msp430-size over 6LoWPAN-ND entities');