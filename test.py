import requests


urls = [
        'http://example.com',
        'http://httpbin.org/get',
        'http://httpbin.org/post',
        'http://httpbin.org/status/200',
        'http://httpbin.org/status/404'
        ]
proxy = {
    'http': 'http://127.0.0.1:8080'
}

json_data = {
    'name': 'bob',
    'age': 25,
}

for url in urls:
    try:
        if 'post' in url:
            response = requests.post(url, json=json_data, proxies=proxy)
            print("Response Status Code:", response.status_code)
            print("Response Content: ", response.json())
        else:
            response = requests.get(url, proxies=proxy)
            print("Response Status Code:", response.status_code)
            print("Response Content: ", response.text)
    except requests.exceptions.RequestException as e:
        print("Error:", e)