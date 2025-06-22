clear, clc
% Setup
duration = 24;
% start_time_target = 4.63;
num_files = 5;
t_step = 0:0.01:duration;
threshold = 0.5;

surface_path = {'plywood', 'PMMA', 'sand'};
freq_path = {'05Hz', '1Hz'};
surface_legend = {'Plywood Surface', 'PMMA Surface', 'Sand Surface'};
freq_legend = {'0.5 Hz', '0.9 Hz'};
colors = ["#0072BD", "#D95319"];  % Left, Right

all_mean_r = zeros(length(t_step), 6);
all_mean_l = zeros(length(t_step), 6);
sur_frq_idx = 1;

figure
hold on
for f_idx = 1:2  % frequency index (1 = 0.5Hz, 2 = 1Hz)
    for s_idx = 1:3  % Surface index
        all_pre_r = zeros(length(t_step), num_files);
        all_pre_l = zeros(length(t_step), num_files);

        for i = 1:5
            % Extract data
            filename = sprintf("data/pressure/%s/motion_log_%s_20ml_%d.csv", ...
                                surface_path{s_idx}, freq_path{f_idx}, i);
            data = readtable(filename);
            
            % Forplay: Align graph. Zero in the y-direction
            set_data_to_zero_r = data.Pressure1_kPa_ - data.Pressure1_kPa_(1);
            set_data_to_zero_l = data.Pressure2_kPa_ - data.Pressure2_kPa_(1);
            
            % Find the index where DAWN actuate. Normally after 4 seconds 
            % and the amplitude of threshold
            start_run_idx_r = find(set_data_to_zero_r >= threshold & ...
                         data.Time_s_ >= 4, 1);
            start_run_idx_l = find(set_data_to_zero_l >= threshold & ...
                         data.Time_s_ >= 4, 1);
            
            % Extract the actual time where it happend
            start_run_time_r = data.Time_s_(start_run_idx_r);
            start_run_time_l = data.Time_s_(start_run_idx_l);
            
            % Time period for recorded pressure and shift by the start time 
            shift_time_r = data.Time_s_ - start_run_time_r;
            shift_time_l = data.Time_s_ - start_run_time_l;
            
            % Find index where the time is positive due to shift
            % Extract its actual time
            actual_start_idx_r = find(shift_time_r >= 0, 1);
            actual_start_idx_l = find(shift_time_l >= 0, 1);
            actual_start_time_r = shift_time_r(actual_start_idx_r);
            actual_start_time_l = shift_time_l(actual_start_idx_l);
            
            % Get index interval between start and end. 
            data_extract_r = find(shift_time_r >= actual_start_time_r & ...
                                shift_time_r <= actual_start_time_r + duration);
            data_extract_l = find(shift_time_l >= actual_start_time_l & ...
                                shift_time_l <= actual_start_time_l + duration);
            
            pre_r = fillmissing(data.Pressure1_kPa_(data_extract_r), 'spline');
            pre_l = fillmissing(data.Pressure2_kPa_(data_extract_l), 'spline');
            % pre_r = data.Pressure1_kPa_(data_extract);
            % pre_l = data.Pressure2_kPa_(data_extract);

            % Zeroing
            pre_r = pre_r - min(pre_r);
            pre_l = pre_l - min(pre_l);
            
            % Remove duplicates - The same time step appears some times.
            
            % Extract actual time in the raw data and shift data
            if f_idx == 2 && s_idx == 2 && i == 6
                t = data.Time_s_(data_extract) - start_time_target + 0.2;
                [t_unique, idx_unique] = unique(t);
            else
                t_r = data.Time_s_(data_extract_r) - start_run_time_r;
                t_l = data.Time_s_(data_extract_l) - start_run_time_l;
                [t_unique_r, idx_unique_r] = unique(t_r);
                [t_unique_l, idx_unique_l] = unique(t_l);
                % t_unique = t_unique - t_unique(1);
                % t_unique = t_unique - t_unique(1);
            end
            
            pre_r = pre_r(idx_unique_r);
            pre_l = pre_l(idx_unique_l);
            
            all_pre_r(:, i) = interp1(t_unique_r, pre_r, t_step, 'linear', 'extrap');
            all_pre_l(:, i) = interp1(t_unique_l, pre_l, t_step, 'linear', 'extrap');
        end

        % Compute mean + std
        mean_r = mean(all_pre_r, 2);
        mean_l = mean(all_pre_l, 2);
        all_mean_r(:, sur_frq_idx) = mean_r;
        all_mean_l(:, sur_frq_idx) = mean_l;
        sur_frq_idx = sur_frq_idx + 1;

        % Plot
        subplot(2, 3, (f_idx-1)*3 + s_idx);
        hold on
        x0=100;
        y0=100;
        width=440;
        height=450;
        set(gcf,'position',[x0,y0,width,height]);
        
        % plot(t_step, all_pre_l, 'LineWidth', 1.2);
        % plot(t_step, all_pre_r, 'LineWidth', 1.2);

        plot(t_step, mean_l, 'LineWidth', 1.2, 'Color', colors(1));
        plot(t_step, mean_r, 'LineWidth', 1.2, 'Color', colors(2));
        grid on
        
        if f_idx == 1
            lgnd = legend('Skin$_L$', 'Skin$_R$', 'Location','northeast','Interpreter','latex');
        else
            lgnd = legend('Skin$_L$', 'Skin$_R$', 'Location','southeast', 'NumColumns', 3, 'Interpreter','latex');
        end
        % fontsize(14,'points')
        % fontsize(lgnd,10,'points')
        set(gca, 'FontSize', 14);
        xlabel('Time [s]')
        ylabel('Pressure [kPa]')
        xlim([0 24]); ylim([-2 12]);
        xticks(0:4:24); yticks(0:2:12);
        title(sprintf('%s - %s', surface_legend{s_idx}, freq_legend{f_idx}), 'Interpreter', 'latex');
        hold off
    end
end

sgtitle('Cyclic Inflation/Deflation – Left & Right Skin Pressure Profiles');

% Optional: Save figure
% saveas(gcf, 'output/six_surfaces_figure.svg');

%% FFT

% Sampling settings
Fs = 100; % Sampling frequency in Hz (from your 0.01s timestep)
N = length(t_step); % Number of samples
f = Fs*(0:(N)-1)/N; % Frequency axis
idx = 1;

figure;
sgtitle('FFT of Mean Pressure Signals');
for f_idx = 1:2
    for s_idx = 1:3
        % Get the averaged signals
        signal_r = all_mean_r(:, idx);
        signal_l = all_mean_l(:, idx);

        % FFT Right
        Y_r = fft(signal_r);
        P_r = abs(Y_r / N);    % Normalize FFT result and get real vals

        % FFT Left
        Y_l = fft(signal_l);
        P_l = abs(Y_l / N);
        
        idx = idx + 1;

        % Plot
        subplot(2, 3, (f_idx-1)*3 + s_idx);
        hold on
        plot(f, P_l, 'Color', colors(1), 'LineWidth', 1.5);
        plot(f, P_r, 'Color', colors(2), 'LineWidth', 1.5);
        grid on
        xlim([0 3]); % limit to 5 Hz for clarity
        ylim([0 6]);
        xlabel('Frequency [Hz]');
        ylabel('|P(f)|');
        title(sprintf('FFT - %s | %s', surface_legend{s_idx}, freq_legend{f_idx}), 'Interpreter', 'latex');
        legend('Left Skin', 'Right Skin');
    end
end


