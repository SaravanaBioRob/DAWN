clear, clc
% Setup
duration = 5;
start_time_target = 6.5;   %4.63
num_files = 5;
threshold = 0.5;
t_step = 0:0.01:duration;

surface_path = {'plywood', 'PMMA', 'sand'};
freq_path = {'05Hz', '1Hz'};
surface_legend = {'Plywood Surface', 'PMMA Surface', 'Sand Surface'};
freq_legend = {'0.5 Hz', '0.9 Hz'};
colors = ["#0072BD", "#D95319", "#77AC30"];  % Left, Right [red, blue, green]

for f_idx = 1:2  % frequency index (1 = 0.5Hz, 2 = 1Hz)
    % Prepare an array to hold all surface data for each frequency
    all_pre_r = zeros(length(t_step), 5);  % 3 surfaces
    all_pre_l = zeros(length(t_step), 5);  % 3 surfaces
    all_pre_r_mean = zeros(length(t_step), 3);  % 3 surfaces
    all_pre_l_mean = zeros(length(t_step), 3);  % 3 surfaces

    for s_idx = 1:3  % Surface index
        % Loop over the 5 files, but since you have only 1 iteration (i = 1), 
        % we'll keep it that way for now.
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
        all_pre_r_mean(:, s_idx) = mean(all_pre_r, 2);
        all_pre_l_mean(:, s_idx) = mean(all_pre_l, 2);
    end

    % Plot data for each frequency (0.5Hz or 1Hz)
    % subplot(1, 2, f_idx);
    figure
    hold on
    for s_idx = 1:3
        plot(t_step, all_pre_l_mean(:, s_idx), 'LineWidth', 1.2, 'DisplayName', surface_legend{s_idx}, 'Color', colors(s_idx));
    end

    x0=10; y0=10; width=6.3; height=6.3;
    set(gcf,'Units','centimeter','position',[x0,y0,width,height]);
    
    lgnd = legend('Location', 'northeast', 'Interpreter', 'latex');
    fontsize(10,'points')
    fontsize(lgnd,8,'points')

    xlabel('Time [s]', 'Interpreter', 'latex')
    ylabel('Pressure [kPa]', 'Interpreter', 'latex')

    xlim([0 4]); 
    ylim([-2 16]);
    xticks(0:1:4); 
    yticks(0:2:16);
    set(gca,'TickLabelInterpreter','latex')

    grid on
    box on
    hold off
end

%sgtitle('Cyclic Inflation/Deflation – Left Skin Pressure Profiles for 0.5 Hz and 0.9 Hz');
% saveas(gcf, 'output/one_cyclic_pressure.svg');