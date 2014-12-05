dir = 'C:\Users\yirong\Documents\同步盘\百度云同步盘\SDN\TCAM\simulaotr\Result\Result\';

Dkey = 64;
Dspeed = 128;
Dvnet = 80;
Dpnet = 980;
%TCAM size
key = [64, 128, 256];
relative_tcam = [];
relative_tcam_5percent = [];
relative_tcam_95percent = [];
for i = 1:length(key)
    tcam = [];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_TCAM.txt',dir,key(i),Dspeed,Dvnet,Dpnet,'IDEAL');
    tcam = [tcam,load(f)];
    tcam = [tcam,load(f)];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_TCAM.txt',dir,key(i),Dspeed,Dvnet,Dpnet,'DMT');
    tcam = [tcam,load(f)];
    f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_TCAM.txt',dir,key(i),Dspeed,Dvnet,Dpnet,'SMT');
    tcam = [tcam,load(f)];
%     f = sprintf('%skeylen%d_speed%d_vnet%d_pnet%d_%s_TCAM.txt',dir,key(i),Dspeed,Dvnet,Dpnet,'OVX');
%     tcam = [tcam,load(f)];
%    tcam(:,1) = tcam(:,2).*tcam(:,3).*tcam(:,4).*tcam(:,5);
    tcam(:,1) = tcam(:,2).*tcam(:,3).*tcam(:,4);
    index = find(tcam(:,1)~=0);
    data = [];
    data = [data, tcam(index,3)./tcam(index,2)];
    data = [data, tcam(index,4)./tcam(index,2)];
    data = sort(data);
    relative_tcam = [relative_tcam; 1, mean(data)];
    relative_tcam_5percent = [relative_tcam_5percent; 1, data(ceil(size(data,1)*0.05),:)];
    relative_tcam_95percent = [relative_tcam_95percent; 1, data(ceil(size(data,1)*0.95),:)];
end
figure;
bar(relative_tcam);
legend('IDEAL','DMT','SMT');
set(gca,'xticklabel',{'64bit','128bit','256bit'});
xlabel('Expected match key length');
ylabel('Relative TCAM occupation per virtual network');