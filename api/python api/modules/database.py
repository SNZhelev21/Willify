import psycopg2
from psycopg2.extras import RealDictCursor

conn = psycopg2.connect("dbname=postgres user=proektna password=kT$Me#sb3WpuNeJ host=10gr-2nd-proj.postgres.database.azure.com port=5432 sslmode=require")

cur = conn.cursor(cursor_factory=RealDictCursor)