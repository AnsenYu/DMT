dir = 'C:\Users\yirong\Documents\同步盘\百度云同步盘\SDN\TCAM\simulaotr\Result\Result\';

Dkey = 128;
Dspeed = 32;
Dvnet = 80;
Dpnet = 980;
ti = sprintf('key:%d speed:%d vnet:%d pnet:%d', Dkey, Dspeed, Dvnet, Dpnet);
%Physical vnetsize distribution
figure;

hold on;
f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_allVnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'IDEAL');
vnetsize = load(f);
[n x] = hist(vnetsize,1:1:max(vnetsize));
plot(x,cumsum(n),'g','LineWidth',4)

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'IDEAL');
vnetsize = load(f);
[n x] = hist(vnetsize,1:1:max(vnetsize));
plot(x,cumsum(n),'y','LineWidth',4)

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'DMT');
vnetsize = load(f);
[n x] = hist(vnetsize,1:1:max(vnetsize));
plot(x,cumsum(n),'r','LineWidth',4);

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'SMT');
vnetsize = load(f);
[n x] = hist(vnetsize,1:1:max(vnetsize));
plot(x,cumsum(n),'b','LineWidth',4)

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_VnetSize.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'OVX');
vnetsize = load(f);
[n x] = hist(vnetsize,1:1:max(vnetsize));
plot(x,cumsum(n),'k','LineWidth',4)



legend('REQUEST','IDEAL','DMT','SMT','OVX');
%set(gca,'xticklabel',{'1','2','4','8','16','32','64','128','256'});
xlabel('success vnet size');
ylabel('CDF of vnetsizes');
title(ti);
