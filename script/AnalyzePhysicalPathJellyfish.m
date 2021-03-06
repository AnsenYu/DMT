dir = 'C:\Users\yirong\Documents\同步盘\百度云同步盘\SDN\TCAM\simulaotr\Result_vnet80_pnet980_jellyfish\Result\';

Dkey = 128;
Dspeed = 64;
Dvnet = 80;
Dpnet = 980;
ti = sprintf('key:%d speed:%d vnet:%d pnet:%d', Dkey, Dspeed, Dvnet, Dpnet);
%Physical path distribution
figure;

hold on;
f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_PhysicalPaths.txt',dir,Dkey,4,Dvnet,Dpnet,'DMT');
path = load(f);
[n x] = hist(path,0:0.01:max(path));
plot(x,cumsum(n)/sum(n),'r','LineWidth',4);

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_PhysicalPaths.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'SMT');
path = load(f);
[n x] = hist(path,0:0.01:max(path));
plot(x,cumsum(n)/sum(n),'b','LineWidth',4)

f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_PhysicalPaths.txt',dir,Dkey,Dspeed,Dvnet,Dpnet,'OVX');
path = load(f);
[n x] = hist(path,0:0.01:max(path));
plot(x,cumsum(n)/sum(n),'k','LineWidth',4)


legend('DMT','SMT','OVX');
%set(gca,'xticklabel',{'1','2','4','8','16','32','64','128','256'});
xlabel('Physical path stretch ratio');
ylabel('CDF of paths');
title(ti);
