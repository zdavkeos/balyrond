function [c,rc] = AppCir(M)
    %
    % Computes an approximate circle in a plane based on (1) parabolic projection
    % of 2D points to 3D, (2) fitting an ordinary least-squares plane in 3D, and
    % (3) projecting the intersecting ellipse to the 2D plane.
    % Input:
    % M: an nx2 matrix of x and y coordinates of n points
    % Output:
    % c: center of the circle; a 1x2 array of x and y coordinates
    % rc: radius of the circle; a scalar
    %
    x = [2*M ones(size(M)(1),1)]\(M(:,1).^2 + M(:,2).^2); % Does the magic!
    c = [x(1) x(2)]; % Center of the circle
    rc = sqrt(x(1)^2 + x(2)^2 + x(3)); % Radius of the circle
end
