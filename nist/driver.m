% Octave driver script to run tests

disp("Running...");

function run_fit_tests (data_fname, fit_fname)
    printf("------------------------------------\n");
    printf("File: %s\n", data_fname);
    fid = fopen(data_fname);
    len = str2num(fgetl(fid));

    M = zeros(len, 3);

    for j=1:len
        M(j,:) = str2num(fgetl(fid));
    end

    fclose(fid);

    % Collapse to 2 dimensions
    if M(1,1) == M(2,1)
        M(:,1) = [];
    elseif M(1, 2) == M(2,2)
        M(:,2) = [];
    else
        M(:,3) = [];
    end

    %[c, rad, f] = A_CL2IC(M);
    %printf("A_CL2IC (MIC) Calculated:\n");
    %printf("  Center: %f\n", c);
    %printf("  Radius: %f\n", rad);

    [c, rad, f] = H_CL2IC(M);
    printf("H_CL2IC (MIC) Calculated:\n");
    printf("  Center: %f\n", c);
    printf("  Radius: %f\n", rad);

    [c, rad, f] = H_CL2CC(M);
    printf("H_CL2CC (MCC) Calculated:\n");
    printf("  Center: %f\n", c);
    printf("  Radius: %f\n", rad);

    % check the "official" solution
    fid = fopen(fit_fname);
    xc = str2num(fgetl(fid));
    yc = str2num(fgetl(fid));
    zc = str2num(fgetl(fid));
    xn = str2num(fgetl(fid)); % x_normal
    yn = str2num(fgetl(fid)); % y_normal
    zn = str2num(fgetl(fid)); % z_normal
    diam = str2num(fgetl(fid));

    printf("Official Least Squares Fit:\n");
    printf("  Center X: %f\n", xc);
    printf("  Center Y: %f\n", yc);
    printf("  Center Z: %f\n", zc);
    printf("  Radius: %f\n\n", diam / 2);
    printf("  Normal: %f %f %f\n", xn, yn, zn);

    
end

run_fit_tests("Circle2d/cir2d1.ds", "Circle2d/cir2d1.fit");
run_fit_tests("Circle2d/cir2d10.ds", "Circle2d/cir2d10.fit");
run_fit_tests("Circle2d/cir2d11.ds", "Circle2d/cir2d11.fit");
run_fit_tests("Circle2d/cir2d12.ds", "Circle2d/cir2d12.fit");
run_fit_tests("Circle2d/cir2d13.ds", "Circle2d/cir2d13.fit");
run_fit_tests("Circle2d/cir2d14.ds", "Circle2d/cir2d14.fit");
run_fit_tests("Circle2d/cir2d15.ds", "Circle2d/cir2d15.fit");
run_fit_tests("Circle2d/cir2d16.ds", "Circle2d/cir2d16.fit");
run_fit_tests("Circle2d/cir2d17.ds", "Circle2d/cir2d17.fit");
run_fit_tests("Circle2d/cir2d18.ds", "Circle2d/cir2d18.fit");
run_fit_tests("Circle2d/cir2d19.ds", "Circle2d/cir2d19.fit");
run_fit_tests("Circle2d/cir2d2.ds", "Circle2d/cir2d2.fit");
run_fit_tests("Circle2d/cir2d20.ds", "Circle2d/cir2d20.fit");
run_fit_tests("Circle2d/cir2d21.ds", "Circle2d/cir2d21.fit");
run_fit_tests("Circle2d/cir2d22.ds", "Circle2d/cir2d22.fit");
run_fit_tests("Circle2d/cir2d23.ds", "Circle2d/cir2d23.fit");
run_fit_tests("Circle2d/cir2d24.ds", "Circle2d/cir2d24.fit");
run_fit_tests("Circle2d/cir2d25.ds", "Circle2d/cir2d25.fit");
run_fit_tests("Circle2d/cir2d26.ds", "Circle2d/cir2d26.fit");
run_fit_tests("Circle2d/cir2d27.ds", "Circle2d/cir2d27.fit");
run_fit_tests("Circle2d/cir2d28.ds", "Circle2d/cir2d28.fit");
run_fit_tests("Circle2d/cir2d29.ds", "Circle2d/cir2d29.fit");
run_fit_tests("Circle2d/cir2d3.ds", "Circle2d/cir2d3.fit");
run_fit_tests("Circle2d/cir2d30.ds", "Circle2d/cir2d30.fit");
run_fit_tests("Circle2d/cir2d4.ds", "Circle2d/cir2d4.fit");
run_fit_tests("Circle2d/cir2d5.ds", "Circle2d/cir2d5.fit");
run_fit_tests("Circle2d/cir2d6.ds", "Circle2d/cir2d6.fit");
run_fit_tests("Circle2d/cir2d7.ds", "Circle2d/cir2d7.fit");
run_fit_tests("Circle2d/cir2d8.ds", "Circle2d/cir2d8.fit");
run_fit_tests("Circle2d/cir2d9.ds", "Circle2d/cir2d9.fit");
