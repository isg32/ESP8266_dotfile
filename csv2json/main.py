import csv
import json
import sys

# Ask user to drag and drop the CSV file
csv_filename = input("Drag and drop your CSV file here and press Enter: ").strip()
json_filename = "output.json"

# Read CSV and convert to JSON
json_data = []
with open(csv_filename, mode='r', encoding='utf-8') as csv_file:
    csv_reader = csv.DictReader(csv_file)
    for row in csv_reader:
        json_data.append({
            "uid": row["UID"].strip(),
            "name": row["Name"].strip()
        })

# Write JSON output
with open(json_filename, mode='w', encoding='utf-8') as json_file:
    json.dump(json_data, json_file, indent=4)

print(f"CSV data has been converted to JSON and saved as {json_filename}")
