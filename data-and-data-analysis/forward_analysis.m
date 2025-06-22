function [v_mean, v_std, v_se, s] = forward_analysis(directory)
    files = dir([directory '\*.txt']);

    for i=1:length(files)
        file = [files(i).folder '\' files(i).name];
        data = importdata(file);

        t(:,i) = data.data(:,1);
        x(:,i) = data.data(:,2);
        y(:,i) = data.data(:,3);
    end

    figure();
    hold on
    plot(x,y)
    axis equal
    hold off

    figure();
    hold on
    plot(t,sqrt(x.^2+y.^2))
    hold off

    dxydt = diff(sqrt(x.^2+y.^2)) ./ diff(t);
    v_avg = mean(dxydt);
    figure();
    hold on
    title("model check")
    subplot(2,1,1)
    histogram(v_avg)
    subplot(2,1,2)
    qqplot(v_avg)
    hold off
    
    v_mean = mean(v_avg);
    v_std = std(v_avg);
    v_se = std(v_avg)/sqrt(length(v_avg));

    s = struct('v_avg',v_avg);
end