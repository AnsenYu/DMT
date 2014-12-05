dir = 'C:\Users\yirong\Documents\同步盘\百度云同步盘\SDN\TCAM\simulaotr\Result\Result\';

Dkey = 128;
Dspeed = 256;
Dvnet = 80;
Dpnet = 980;
%Max concurrent virtual network
speed = [4 8 16 32 64 128 256];

ti = sprintf('key:%d speed:%d vnet:%d pnet:%d', Dkey, Dspeed, Dvnet, Dpnet);

mcv_all = [];
for i = 1:length(speed)
    mcv = [];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_AcceptRatio.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'DMT');
    mcv = [mcv,load(f)];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_AcceptRatio.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'DMTe2e');
    mcv = [mcv,load(f)];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_AcceptRatio.txt',dir,Dkey,speed(i),Dvnet,Dpnet,'DMTe3e');
    mcv = [mcv,load(f)];     
    mcv_all = [mcv_all; mcv];
end
figure;
plot(mcv_all);
legend('DMT','DMTe2e','DMTe3e');
set(gca,'xticklabel',{'4','8','16','32','64','128','256'});
xlabel('Expected arriving rate of vnet request');
ylabel('Accept ratio of serving virtual networks');
title(ti);