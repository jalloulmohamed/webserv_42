#!/usr/bin/env python3
import os
import sys
import re
from urllib.parse import parse_qs

def parse_input():
    if "REQUEST_METHOD" in os.environ:
        if os.environ["REQUEST_METHOD"] == "POST":
            # Read the raw POST data
            content_length = int(os.environ.get("CONTENT_LENGTH", 0))
            post_data = sys.stdin.read(content_length)
            # Parse the raw POST data into key-value pairs
            pattern = r'name="([^"]+)"\s*\r?\n\r?\n([^-\s]+)'
            matches = re.findall(pattern, post_data)
            keys_and_values = {}
            for match in matches:
                key = match[0]
                value = match[1]
                keys_and_values[key] = value
            print(keys_and_values)
        elif os.environ["REQUEST_METHOD"] == "GET":
            # Parse the query string into key-value pairs
            query_string = os.environ["QUERY_STRING"]
            keys_and_values = parse_qs(query_string)
            print(keys_and_values)

if __name__ == "__main__":
    parse_input()
