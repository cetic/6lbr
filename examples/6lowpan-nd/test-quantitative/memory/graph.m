%% data
x = {'6lh'; '6lr'; '6lbr'; 'rpl'; '6lr-rpl'; '6lbr-rpl'};
y = [ 31848 168 5992 ; 36534 168 6148 ; 32150 168 6720 ; 43889 192 6182 ; 48942 192 6552 ; 44668 192 7124 ];
% enddata

d = [y(:,1) zeros(size(y)) y(:,2:3) zeros(size(y))];
d = reshape(d',3,[])'


%% Construct graph
figure;
bar(d,'stacked')
set(gca,'XLim',[0 18],'XTick',1:3:17,'XTickLabels',x);

hold on;
x = 0:length(d)+1;
plot(x,ones(1,length(x)).*(48*1024), 'b')

hold on;
plot(x,ones(1,length(x)).*(10*1024), 'r')

 
legend('Code','Initialized variables','Uninitialized variables', 'max ROM', 'max RAM', 2);
xlabel('Entity');
ylabel('memory (bytes)');
title('Size of ROM and RAM used in 6LoWPAN-ND entities');
