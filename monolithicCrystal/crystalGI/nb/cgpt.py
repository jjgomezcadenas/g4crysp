from openai import OpenAI
opai_key ="sk-j6Ok4wZi-LmDJLNddLN0lLkTKjdor95kFolULU9TgaT3BlbkFJi07SRkmoqjGtUjag49jCs7z07og8PjVDEIBh_lgIcA"

client = OpenAI(api_key=opai_key)
import json


# Define the messages for the conversation
messages = [
    {
        "role": "system",
        "content": "You are a helpful assistant for programming tasks."
    },
    {
        "role": "user",
        "content": "I have a pandas DataFrame with columns: 'event', 'ntrk', 'proc', 'x1', 'y1', 'z1', 'e1', 'x2', 'y2', 'z2', 'e2', 'x12', 'y12', 'z12', 'xb', 'yb', 'zb'. Compute extra columns: 'd12' and 'd12b'. 'd12' is the distance between the points defined by (x1, y1, z1) and (x2, y2, z2), and 'd12b' is the distance between the points defined by (x12, y12, z12) and (xb, yb, zb). Provide Python code for this task."
    }
]

# Make the API request
response = client.chat.completions.create(model="gpt-4o-mini",  # or "gpt-3.5-turbo"
messages=messages,
max_tokens=300,  # Adjust for the desired output length
temperature=0.0  # Lower temperature for more deterministic output)
)

# Extract and print the response content
print(response.choices[0].message.content.strip())
