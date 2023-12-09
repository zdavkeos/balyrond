function [c,rc,f] = H_CL2IC(M)
    %
    % Implementation of a heuristic
    % to fit a constrained (inscribed) least-squares circle in a plane
    % Input:
    % M: an nx2 matrix of x and y coordinates of n points
    % Output:
    % c: center of the circle; a 1x2 array of x and y coordinates
    % rc: radius of the circle; a scalar
    % f: minimized objective function; a scalar
    %
    % Needs:
    % AppCir: an external function to get an approximate circle
    % fminsearch: a built-in function to search for minimum
    % CL2IC_ObjFun: a local function needed by fminsearch
    %
    global P % Make the coordinate matrix a global variable in this file
    P = M; % Copy M to P
    [x0, apprad] = AppCir(P); % Get the approximate circle to start the search
    [c, f] = fminsearch(@CL2IC_ObjFun, x0); % Conduct the heuristic search
                                            % Input:
                                            % @CL2IC_ObjFun: handle for a local objective function
                                            % x0: a 1x2 array of starting circle center coordinates
                                            % Output:
                                            % c: a 1x2 array of circle center coordinates
                                            % f: minimized objective function; a scalar
    rc = min(sqrt((P(:,1)-c(1)).^2 + (P(:,2)-c(2)).^2)); % Radius of the circle
end
%
function f = CL2IC_ObjFun(x)
    % Objective function needed by fminsearch
    % Input:
    % x: starting approximation for center and radius
    % center is a 1x2 array of x and y coordinates
    % radius is a scalar
    % Output:
    % f: Objective function = square root of the mean of the squares (RMS)
    % of the deviations of the points from the circle; a scalar
    %
    global P % a global variable to get the coordinate matrix
    r = sqrt((P(:,1)-x(1)).^2 + (P(:,2)-x(2)).^2); % radius vector
    f = (norm(r-min(r)))/sqrt(size(r)(1)); % RMS of deviations from the circle
end
