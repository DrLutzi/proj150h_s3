% here define o1 and o2 as orders of bezier surfaces
o1=1;
o2=2;

n1 = o1 + 1;
n2 = o2 + 1;
oT = o1 + o2;
nT = oT + 1;

G = ones([n1 n2]);
T = ones([nT nT]);

syms a;
syms b;
syms c;
%c = 1 - a - b;

GCoefs = sym('gc', [n1 n2]);
for j1_Index = 1:n1
    for j2_Index = 1:n2
        j1=j1_Index-1;
        j2=j2_Index-1;
        GCoefs(j1_Index, j2_Index) = G(j1_Index, j2_Index) * nchoosek(o1,j1)*(b+c).^(j1) * a.^(o1-j1) * nchoosek(o2, j2)*(a+c).^(j2) * b.^(o2-j2);
    end
end

disp(expand(transpose(GCoefs)));

%lower part of the triangle
TCoefs = sym('tc', [nT nT]);
for i3_Index = 1:nT
    for i2_Index = 1:nT-i3_Index+1
        i1_Index = nT-i3_Index-i2_Index+2;
        i1=i1_Index-1;
        i2=i2_Index-1;
        i3=i3_Index-1;
        TCoefs(i1_Index, i2_Index) = T(i1_Index, i2_Index) * (factorial(oT)/(factorial(i1)*factorial(i2)*factorial(i3))) * a.^(i1) * b.^(i2) * c.^(i3);
    end
end

disp(expand(transpose(TCoefs)));