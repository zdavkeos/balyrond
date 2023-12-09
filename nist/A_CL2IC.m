function [c,rc,f] = A_CL2IC ( M )
%
% Implementation of an algorithm
% to fit a constrained (inscribed) least-squares circle in a plane
% Input:
% M: an nx2 matrix of x and y coordinates of n points
% Output:
% c: center of the circle; a 1x2 array of x and y coordinates
% rc: radius of the circle; a scalar
% f: minimized objective function; a scalar
%
% Needs:
% fminbnd: a built-in function to search for minimum over a Voronoi edge
% CL2IC_ObjFun: a local function needed by fminbnd
%
    global P % Make the coordinate matrix a global variable in this file
    global psedge % Make starting endpoint of an edge global
    global pfedge % Make finishing endpoint of an edge global
    P = M; % Copy M to P
    [vx, vy] = voronoi(P(:,1), P(:,2)); % Compute (nearest-neighbor) Voronoi diagram
    nedge = size(vx)(2); % Number of edges in the Voronoi diagram
    ps = [vx(1,:); vy(1,:)]; % Vector of starting endpoints of Voronoi edges
    pf = [vx(2,:); vy(2,:)]; % Vector of finishin endpoints of Voronoi edges
    fminval = zeros(nedge);
    cen = 
    for j=1:nedge % Search over all Voronoi edges
        psedge = ps(:,j); % Starting endpoint of Voronoi edge
        pfedge = pf(:,j); % Finishing endpoint of Voronoi edge
        [tval, fval] = fminbnd(@CL2IC_ObjFun, 0, 1); % Call the built-in minimizer
        cen(:,j) = (1-tval)*psedge + tval*pfedge; % Center coordinates of local minima
        fminval(j) = fval; % Vector of local minima
    end

    [f, ednum] = min(fminval); % Global minimum
    c = cen(:,ednum); % Center coordinates of global minimum
    rc = min(sqrt((P(:,1)-c(1)).^2 + (P(:,2)-c(2)).^2)); % Radius at global minimum
end

function f = CL2IC_ObjFun(t)
% Objective function needed by fminbnd
% Input:
% t: argument (parameter) for the objective function; a scalar
% Output:
% f: Objective function = square root of the mean of the squares (RMS)
% of the deviations of the points from the circle; a scalar
%
    global P % a global variable to get the coordinate matrix
    global psedge % Make starting endpoint of an edge global
    global pfedge % Make finishing endpoint of an edge global
    x = (1-t)*psedge + t*pfedge; % From parameter to coordinates
    r = sqrt((P(:,1)-x(1)).^2 + (P(:,2)-x(2)).^2); % radius vector
    f = (norm(r-min(r)))/sqrt(size(r)(1)); % RMS of deviations from the circle
end
