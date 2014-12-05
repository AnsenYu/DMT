dir = 'C:\Users\yirong\Documents\同步盘\百度云同步盘\SDN\TCAM\simulaotr\Result\Result\';

Dkey = 256;
Dspeed = 256;
Dvnet = 80;
Dpnet = 980;
%Max concurrent virtual network
speed = [1 2 4 8 16 32 64 128 256];

ti = sprintf('key:%d speed:%d vnet:%d pnet:%d', Dkey, Dspeed, Dvnet, Dpnet);

mcv_all = [];
for i = 1:length(speed)
    mcv = [];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_MaxConcurrentVnets.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'IDEAL');
    mcv = [mcv,load(f)];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_MaxConcurrentVnets.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'DMT');
    mcv = [mcv,load(f)];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_MaxConcurrentVnets.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'SMT');
    mcv = [mcv,load(f)];     
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_MaxConcurrentVnets.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'OVX');
    mcv = [mcv,load(f)];
    mcv_all = [mcv_all; mcv];
end
figure;
plot(mcv_all);
legend('IDEAL','DMT','SMT','OVX');
set(gca,'xticklabel',{'1','2','4','8','16','32','64','128','256'});
xlabel('Expected arriving rate of vnet request');
ylabel('Maximum number of concurrent serving virtual networks');
title(ti);