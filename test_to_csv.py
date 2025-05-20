import os
import csv
import re

# Directory containing the log files
log_dir = os.path.join(os.path.dirname(__file__), 'branch_logs')

# Output CSV file
output_csv = 'csv/branch_results.csv'

# Regex to parse filename
filename_re = re.compile(r'^(.*?)\.(\d+)\.(\d+)\.(\d+)\.(\d+)\.log\.txt$')

# Regex to parse the line in the file
line_re = re.compile(r'CPU 0 Branch Prediction Accuracy: ([\d.]+)% MPKI: ([\d.]+) Average ROB Occupancy at Mispredict: ([\d.]+)')

rows = []

for fname in os.listdir(log_dir):
    match = filename_re.match(fname)
    if not match:
        continue
    test, global_hist, ip_hist, tendency_bits, gs_hist = match.groups()
    file_path = os.path.join(log_dir, fname)
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
        deadlock = any('DEADLOCK!' in line for line in lines)
        deadlock = int(deadlock)
        if not lines:
            # File is empty, save NaN values
            rows.append([
                test, global_hist, ip_hist, tendency_bits, gs_hist, 'NaN', 'NaN', 'NaN', deadlock
            ])
            continue
        # Search for the first line matching the accuracy/MPKI/ROB pattern
        found = False
        for line in lines:
            line = line.strip()
            if not line:
                continue
            line_match = line_re.match(line)
            if line_match:
                accuracy, mpki, rob_occ = line_match.groups()
                rows.append([
                    test, global_hist, ip_hist, tendency_bits, gs_hist, accuracy, mpki, rob_occ, deadlock
                ])
                found = True
                break
        if not found:
            # No matching line found, save NaN values
            rows.append([
                test, global_hist, ip_hist, tendency_bits, gs_hist, 'NaN', 'NaN', 'NaN', deadlock
            ])

# Write to CSV
with open(output_csv, 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([
        'TEST', 'GLOBAL_HISTORY_LENGTH', 'IP_HISTORY_TABLE_SIZE', 'TENDENCY_BITS', 'GS_HISTORY_TABLE_SIZE',
        'Branch Prediction Accuracy (%)', 'MPKI', 'Average ROB Occupancy at Mispredict', 'deadlock'
    ])
    writer.writerows(rows)

print(f"CSV file '{output_csv}' created with {len(rows)} rows.")
