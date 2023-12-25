from App import app

@app.route('/api/hello', methods=['GET'])
def get_data():
    return {"message": "merge de acum"}