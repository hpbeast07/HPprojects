from flask import Flask, request, render_template_string
from twilio.rest import Client

app = Flask(__name__)

# Apne credentials yahan daalo (test ke liye)
account_sid = "ACa8b2b5e3dc6b56e59f9ebc1a858078ad"
auth_token = "ab8fd99be84f1a76e74d1e58a56d7718"

client = Client(account_sid, auth_token)

HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>Phone Lookup</title>
</head>
<body style="font-family:Arial; padding:40px;">

<h2>Twilio Phone Lookup</h2>

<form method="POST">
    <input type="text"
           name="number"
           placeholder="Enter mobile number"
           required>
    <button type="submit">Lookup</button>
</form>

{% if data %}
<hr>

<p><b>📱 Number:</b> {{ data.mobile }}</p>
<p><b>🌍 Country Code:</b> {{ data.country }}</p>
<p><b>📡 Carrier:</b> {{ data.carrier }}</p>
<p><b>📞 Line Type:</b> {{ data.line_type }}</p>
<p><b>ℹ️ Status:</b> {{ data.status }}</p>

{% endif %}

</body>
</html>
"""

def fetch_data(number):
    try:
        # Agar user ne +91 nahi likha
        if not number.startswith("+"):
            number = "+91" + number

        phone = client.lookups.v2.phone_numbers(number).fetch(
            fields="line_type_intelligence"
        )

        # Debug output
        print("\n========== RAW RESPONSE ==========")
        print(phone.__dict__)

        info = getattr(phone, "line_type_intelligence", None)
        print("\n========== LINE TYPE INFO ==========")
        print(info)
        print("===================================\n")

        country = getattr(phone, "country_code", "Unknown")
        carrier = "Unknown"
        line_type = "Unknown"

        if info:
            # Dict ya object dono handle karega
            if isinstance(info, dict):
                carrier = info.get("carrier_name", "Unknown")
                line_type = info.get("type", "Unknown")
            else:
                carrier = getattr(info, "carrier_name", "Unknown")
                line_type = getattr(info, "type", "Unknown")

        return {
            "mobile": number,
            "country": country,
            "carrier": carrier,
            "line_type": line_type,
            "status": "Lookup completed"
        }

    except Exception as e:
        print("ERROR:", e)

        return {
            "mobile": number,
            "country": "N/A",
            "carrier": "N/A",
            "line_type": "N/A",
            "status": str(e)
        }

@app.route("/", methods=["GET", "POST"])
def home():
    data = None

    if request.method == "POST":
        number = request.form.get("number", "").strip()
        data = fetch_data(number)

    return render_template_string(HTML, data=data)

if __name__ == "__main__":
    app.run(debug=True)