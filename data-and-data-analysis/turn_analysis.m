function [omega_mean, omega_std, omega_se, r_mean, r_std, r_se] = turn_analysis(directory)
    folders = ['1' '2' '3' '4' '5'];
    for i=1:length(folders)
        t_ = [];
        x_ = [];
        y_ = [];
        files = dir([directory folders(i) '/*.txt']);
        rold = 0;
        for j=1:length(files)
            t = [];
            x = [];
            y = [];
            file = [files(j).folder '\' files(j).name];
            data = importdata(file);
        
            t(:,1) = data.data(:,1);
            x(:,1) = data.data(:,2);
            y(:,1) = data.data(:,3);
            
            [r,xc,yc,err]=circfit(x,y);
            if (rold < r)
                rold  = r;
                r_(i) = r;
                xc_(i) = xc;
                yc_(i) = yc;
                err_(i) = err;
                t_ = t;
                x_ = x;
                y_ = y;
            end  
        end
        tComplete(1:length(t_),i) = t_;
        xComplete(1:length(x_),i) = x_ - xc_(i);
        yComplete(1:length(y_),i) = y_ - yc_(i);
    end
    
    theta = atan2(yComplete,xComplete);
    dthetadt = diff(abs(theta))./diff(tComplete);
    
    omega_avg = mean(abs(dthetadt),'omitnan');
    
    figure();
    hold on
    title("model check")
    subplot(2,1,1)
    histogram(omega_avg)
    subplot(2,1,2)
    qqplot(omega_avg)
    hold off
    
    omega_mean = mean(omega_avg);
    omega_std = std(omega_avg);
    omega_se = omega_std/sqrt(length(omega_avg));
    
    r_mean = mean(r_);
    r_std = std(r_);
    r_se = r_std/sqrt(length(r_));

end