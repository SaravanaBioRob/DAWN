function [data_mat, t, s] = extract_friction_data(directory, ts, te, p_state)

    files = dir([directory '\*.csv']);
    data_mat = [];
    f_intercept = [];
    f_slope = [];
    f_avg = [];

    for i = 1:length(files)
        file = [files(i).folder '\' files(i).name];
        data = readtable(file);
        if te == 0
            te_ = length(data.Var1);
        else
            te_ = te;
        end
        data_mat(end+1,:) = data.Var2(1:end);

        mdl = fitlm(data.Var1(ts:te_),data.Var2(ts:te_));
        f_intercept(end+1) = mdl.Coefficients.Estimate(1);
        f_slope(end+1) = mdl.Coefficients.Estimate(2);
        f_avg(end+1) = mean(data.Var2(ts:te_));

        if p_state
            figure()
            plot(mdl)
        else
            
        end

    end
    t = data.Var1(1:end);

    field1 = 'imean'; intercept_mean = mean(f_intercept);
    field2 = 'istd'; intercept_std = std(f_intercept);
    field3 = 'ise'; intercept_se = std(f_intercept)/sqrt(length(f_intercept));

    field4 = 'amean'; avg_mean = mean(f_avg);
    field5 = 'astd'; avg_std = std(f_avg);
    field6 = 'ase'; avg_se = std(f_avg)/sqrt(length(f_avg));
    
    field7 = 'smean'; slope_mean = mean(f_slope);

    field8 = 'i';
    field9 = 'a';

    s = struct(field1,intercept_mean,field2,intercept_std,field3,intercept_se,field4,avg_mean,field5,avg_std,field6,avg_se,field7,slope_mean,field8,f_intercept,field9,f_avg);
end