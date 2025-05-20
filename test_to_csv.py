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
    with open(os.path.join(log_dir, fname), 'r', encoding='utf-8') as f:
        line = f.readline().strip()
        if not line:
            # File is empty, save NaN values
            rows.append([
                test, global_hist, ip_hist, tendency_bits, gs_hist, 'NaN', 'NaN', 'NaN'
            ])
            continue
        line_match = line_re.match(line)
        if not line_match:
            continue
        accuracy, mpki, rob_occ = line_match.groups()
        rows.append([
            test, global_hist, ip_hist, tendency_bits, gs_hist, accuracy, mpki, rob_occ
        ])

# Write to CSV
with open(output_csv, 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow([
        'TEST', 'GLOBAL_HISTORY_LENGTH', 'IP_HISTORY_TABLE_SIZE', 'TENDENCY_BITS', 'GS_HISTORY_TABLE_SIZE',
        'Branch Prediction Accuracy (%)', 'MPKI', 'Average ROB Occupancy at Mispredict'
    ])
    writer.writerows(rows)

print(f"CSV file '{output_csv}' created with {len(rows)} rows.")
