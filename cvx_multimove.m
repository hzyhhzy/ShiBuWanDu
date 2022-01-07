h=6;
w=5;

N=15;

cvx_begin
    variables x(4,h,w) y(h,w);%x=每个表盘触发了4个方向分别多少次,y=每个表盘点击了多少次
    %x(1),x(2),x(3),x(4)分别是右，下，左，上
    xsum=reshape(sum(x,1),[h,w]);%每个表盘转了多少次
    maximize sum(sum(sum(x(:,:,:))));%总转动次数
    subject to
        x>=0;%naive
        y>=0;%naive
        sum(sum(y))==N;%点击了多少次
        xsum==[zeros(h,1),reshape(x(1,:,1:(w-1)),[h,w-1])]+...
            [zeros(1,w);reshape(x(2,1:(h-1),:),[h-1,w])]+...
            [reshape(x(3,:,2:w),[h,w-1]),zeros(h,1)]+...
            [reshape(x(4,2:h,:),[h-1,w]);zeros(1,w)]+...
            y; %每个表盘转动次数等于四个方向的触发加上点击次数
        x(1,:)>=x(2,:);%几条有关转动的约束
        x(2,:)>=x(3,:);
        x(3,:)>=x(4,:);
        x(1,:)-x(4,:)<=1;
    
cvx_end