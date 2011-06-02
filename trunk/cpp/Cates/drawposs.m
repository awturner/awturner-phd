clear; close; clc;
figure; hold on;
scatter3x([225.491 212.102 47.9938]', 'ko' );
ps = [
  224.767 212.488 48.2504
  226.198 211.725 47.7429
]';
plot3x(ps, 'r');
% vertex 4896
text( 224.767, 212.488, 48.2504, '88.2969' );
% vertex 4842
text( 226.198, 211.725, 47.7429, '87.1065' );
ps = [
  226.198 211.725 47.7429
  225.49 211.702 48.7074
]';
plot3x(ps, 'r');
% vertex 4842
text( 226.198, 211.725, 47.7429, '87.1065' );
% vertex 4957
text( 225.49, 211.702, 48.7074, '87.1423' );
ps = [
  225.49 211.702 48.7074
  224.767 212.488 48.2504
]';
plot3x(ps, 'r');
% vertex 4957
text( 225.49, 211.702, 48.7074, '87.1423' );
% vertex 4896
text( 224.767, 212.488, 48.2504, '88.2969' );

% best on face 9656
% ds = 87.1065, 87.1423
scatter3( 225.845, 211.714, 48.2235, 'b+');
text( 225.845, 211.714, 48.2235, '87.1223' );
% best on face 9783
% ds = 87.1423, 88.2969
% dist = 87.9604, ds[0] = 87.1423, s2c = 87.1423
scatter3( 225.49, 211.702, 48.7074, 'b+');
text( 225.49, 211.702, 48.7074, '87.1423' );
quiver3( 225.491, 212.102, 47.9938, 0.354636, -0.388675, 0.229746, 0, 'k:');
axis equal
