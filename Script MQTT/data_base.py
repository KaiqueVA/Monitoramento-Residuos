import sqlite3

def connect_db():
    try:
        connection = sqlite3.connect("Contentor.db")
        return connection
    except sqlite3.Error as error:
        print("Erro ao conectar ao banco de dados:", error)
        return None
    
    
def create_table():
    connection = connect_db()
    if connection:
        try:
            cursor = connection.cursor()
            create_table_query = """
            CREATE TABLE IF NOT EXISTS dados_lorawan (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                devaddr TEXT,
                battery INTEGER,
                volume REAL,
                is_tipped_over BOOLEAN,
                latitude REAL,
                longitude REAL,
                timestamp TEXT,
                rssi TEXT
            );
            """
            cursor.execute(create_table_query)
            connection.commit()
            print("Tabela criada (ou já existe).")
        except sqlite3.Error as error:
            print(f"Erro ao criar a tabela: {error}")
        finally:
            cursor.close()
            connection.close()
            
def insert_data_to_db(devaddr, battery, volume, is_tipped_over, latitude, longitude, timestamp, rssi):
    connection = connect_db()
    if connection:
        try:
            cursor = connection.cursor()
            insert_query = """
            INSERT INTO dados_lorawan (devaddr, battery, volume, is_tipped_over, latitude, longitude, timestamp, rssi)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            """
            cursor.execute(insert_query, (devaddr, battery, volume, is_tipped_over, latitude, longitude, timestamp, rssi))
            connection.commit()
            print("Dados inseridos com sucesso.")
        except sqlite3.Error as error:
            print(f"Erro ao inserir dados: {error}")
        finally:
            cursor.close()
            connection.close()
