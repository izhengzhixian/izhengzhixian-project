#!/usr/bin/env bash
rm *.sqlite3
python manage.py makemigrations
python manage.py makemigrations west
python manage.py migrate --database=west_db
python manage.py migrate
