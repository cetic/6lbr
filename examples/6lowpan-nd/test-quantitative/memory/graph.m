% data
x = {'6lh'; '6lr'; '6lbr'};
y = [ 37305 184 7022 ; 41183 184 7178 ; 37701 184 7750 ];
% enddata

% Construct graph
figure;
bar(y,'stacked');
set(gca,'XTickLabels',x); 
legend('Code','Initialized variables','Uninitialized variables');
xlabel('Entity');
ylabel('memory (bytes)');
title('msp430-size over 6LoWPAN-ND entities');