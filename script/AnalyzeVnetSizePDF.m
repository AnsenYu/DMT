dir = 'C:\Users\yirong\Documents\同步盘\百度云同步盘\SDN\TCAM\simulaotr\Result\Result\';

Dkey = 128;
Dspeed = 128;
Dvnet = 80;
Dpnet = 980;
ti = sprintf('key:%d speed:%d vnet:%d pnet:%d', Dkey, Dspeed, Dvnet, Dpnet);
%Physical vnetsize distribution
figure;

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_allVnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'IDEAL');
vnetsize = load(f);
[n x] = hist(vnetsize,2:1:max(vnetsize));
plot(x,n/sum(n),'g','LineWidth',4)

hold on;
f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,4,Dvnet,Dpnet,'DMT');
vnetsize = load(f);
[n x] = hist(vnetsize,2:1:max(vnetsize));
plot(x,n/sum(n),'r','LineWidth',4);

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'SMT');
vnetsize = load(f);
[n x] = hist(vnetsize,2:1:max(vnetsize));
plot(x,n/sum(n),'b','LineWidth',4)

% f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'OVX');
% vnetsize = load(f);
% [n x] = hist(vnetsize,2:1:max(vnetsize));
% plot(x,n/sum(n),'k','LineWidth',4)



legend('REQUEST','DMT','SMT');
%set(gca,'xticklabel',{'1','2','4','8','16','32','64','128','256'});
xlabel('success vnet size');
ylabel('PDF of vnetsizes');
title(ti);
