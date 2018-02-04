#-*- coding:utf-8 -*-

from django.shortcuts import render

# Create your views here.

from django.http import HttpResponse
from west.models import Student
from west.models import Card
from west.models import Book
from west.models import Book_Borrow
from west.models import Consume
from west.models import Guard
from west.models import Guard_record
from west.models import CCMSAdmin
from django.views.decorators.csrf import csrf_exempt
from django.utils import timezone

import datetime
import json
from decimal import Decimal


def init_data(request):
    admin = CCMSAdmin(username="all", password="all", flag="all")
    admin.save()
    book = CCMSAdmin(username="book", password="book", flag="book")
    book.save()
    student = CCMSAdmin(username="student", password="student", flag="student")
    student.save()
    card = CCMSAdmin(username="card", password="card", flag="card")
    card.save()
    return ret_succeed()


Error = {
    "Succeed": 0,
    "Not Post": 1,
    "Not have action": 2,
    "Values not complete": 3,
    "Values type error": 4,
    "Unknown error": 5,
    "Exist student": 6,
    "Not exist student": 7,
    "Not exist card": 8,
    "Exist book": 9,
    "Not exist book": 10,
    "Book not in library": 11,
    "Not exist borrow book record": 12,
    "Exist guard": 13,
    "Not exist guard": 14,
    "Close card": 15,
    "Book bno clash": 16,
    "Not exist username or password": 17,
    "Money must greater than 0": 18,
    "Not sufficient funds": 19,
}


def ret_error(error_msg):
    if not Error.get(error_msg):
        error_msg = "Unknown error"
    ret = {
        'code': Error[error_msg],
        'message': error_msg,
    }
    return HttpResponse(json.dumps(ret))


def ret_succeed(extend={}):
    error_msg = "Succeed"
    ret = {
        'code': Error[error_msg],
        'message': error_msg,
    }
    ret.update(extend)
    return HttpResponse(json.dumps(ret))


@csrf_exempt
def ccms_api(request):
    if request.method != 'POST':
        return ret_error("Not Post")
    req_json = json.loads(request.body)
    action = req_json['action']
    if not isinstance(req_json.get('values'), dict):
        return ret_error("Values type error")
    if action == "login":
        return login(req_json.get('values'))
    elif action == 'kaihu':
        return kaihu(req_json.get('values'))
    elif action == 'guashi':
        return guashi(req_json.get('values'))
    elif action == 'buka':
        return buka(req_json.get('values'))
    elif action == 'zhuxiao':
        return zhuxiao(req_json.get('values'))
    elif action == 'chongzhi':
        return chongzhi(req_json.get('values'))
    elif action == 'dengji':
        return dengji(req_json.get('values'))
    elif action == 'jieyue':
        return jieyue(req_json.get('values'))
    elif action == 'xujie':
        return xujie(req_json.get('values'))
    elif action == 'guihuan':
        return guihuan(req_json.get('values'))
    elif action == 'book_chaxun':
        return book_chaxun(req_json.get('values'))
    elif action == 'consume':
        return consume(req_json.get('values'))
    elif action == 'consume_chaxun':
        return consume_chaxun(req_json.get('values'))
    elif action == 'open_menjin':
        return open_menjin(req_json.get('values'))
    elif action == 'close_menjin':
        return close_menjin(req_json.get('values'))
    elif action == 'menjin_chaxun':
        return menjin_chaxun(req_json.get('values'))
    else:
        return ret_error("Not have action")


def login(values):
    username = values.get('username')
    password = values.get('password')
    if not username or not password:
        return ret_error("Values not complete")
    if len(CCMSAdmin.objects.filter(username=username, 
        password=password)) == 0:
        return ret_error("Not exist username or password")
    user = CCMSAdmin.objects.get(username=username,
            password=password)
    ret = {'values': {'identify': {'position': user.flag, 'flag': ''}}}
    return ret_succeed(ret)


def kaihu(values):
    xuehao = values.get('xuehao')
    xingming = values.get('xingming')
    yuanxi = values.get('yuanxi')
    if not xuehao or not xingming or not yuanxi:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) > 0:
        return ret_error("Exist student")
    stu = Student(sno=xuehao, sname=xingming, s_class=yuanxi)
    stu.save()
    buka(values)
    return ret_succeed()


def guashi(values):
    xuehao = values.get('xuehao')
    if not xuehao:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    Card.objects.filter(sno=xuehao).update(active=0)
    return ret_succeed()


def buka(values):
    xuehao = values.get('xuehao')
    if not xuehao:
        return ret_error("Values not complete")
    if len(Card.objects.filter()) == 0:
        cardid = 1
    else:
        cardid = max(Card.objects.values('cardID'))['cardID']
        cardid = int(cardid) + 1
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    new_balance = Decimal('0')
    if len(Card.objects.filter(sno=xuehao)) > 0:
        new_balance = Card.objects.get(sno=xuehao).balance
        Card.objects.filter(sno=xuehao).update(active=0, sno=None)
    card = Card(cardID=str(cardid), balance=new_balance, sno=xuehao, active=1)
    card.save()
    return ret_succeed()


def zhuxiao(values):
    xuehao = values.get('xuehao')
    if not xuehao:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    Card.objects.filter(sno=xuehao).delete()
    Student.objects.filter(sno=values.get('xuehao')).delete()
    return ret_succeed()


def chongzhi(values):
    xuehao = values.get('xuehao')
    jine = values.get('jine')
    if not isinstance(jine, (int, float)):
        return ret_error("Values type error")
    if jine <= 0:
        return ret_error("Money must greater than 0")
    if not xuehao or not jine:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    card = Card.objects.get(sno=xuehao)
    if card.active == 0:
        return ret_error("Close card")
    new_balance = card.balance + Decimal(str(jine))
    Card.objects.filter(sno=xuehao).update(balance=new_balance)
    return ret_succeed()


def dengji(values):
    new_isbn = values.get('isbn')
    new_bno = values.get('bno')
    new_bname = values.get('bname')
    new_bauthor = values.get('bauthor')
    new_bpress = values.get('bpress')
    if not new_isbn or not new_bno or not new_bname or not \
       new_bauthor or not new_bpress:
        return ret_error("Values not complete")
    if len(Book.objects.filter(isbn=new_isbn)) > 0:
        return ret_error("Exist book")
    if len(Book.objects.filter(bno=new_bno)) > 0:
        same_book = Book.objects.filter(bno=new_bno)[0]
        if same_book.bname != new_bname or same_book.bauthor != new_bauthor \
                or same_book.bpress != new_bpress:
            return ret_error("Book bno clash")

    book = Book(isbn=new_isbn, bno=new_bno, bname=new_bname,
                bauthor=new_bauthor, bpress=new_bpress)
    book.save()
    return ret_succeed()


def jieyue(values):
    xuehao = values.get('xuehao')
    suoyinhao = values.get('suoyinhao')
    if not xuehao or not suoyinhao:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    if len(Book.objects.filter(isbn=suoyinhao)) == 0:
        return ret_error("Not exist book")
    book = Book.objects.get(isbn=suoyinhao)
    if book.status == 0:
        return ret_error("Book not in library")

    card = Card.objects.get(sno=xuehao)
    if card.active == 0:
        return ret_error("Close card")
    suoyinhao = Book.objects.get(isbn=suoyinhao)
    cardid = card
    now_time = datetime.datetime.now()
    now_time_30days = now_time + datetime.timedelta(days=30)
    borrow = Book_Borrow(isbn=suoyinhao, cardID=cardid,
                         bdate=now_time, rdate=now_time_30days, flag=0)
    borrow.save()
    book.status = 0
    book.save()
    return ret_succeed()


def xujie(values):
    xuehao = values.get('xuehao')
    suoyinhao = values.get('suoyinhao')
    if not xuehao or not suoyinhao:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    card = Card.objects.get(sno=xuehao)
    if card.active == 0:
        return ret_error("Close card")
    cardid = card.cardID
    if len(Book.objects.filter(isbn=suoyinhao)) == 0:
        return ret_error("Not exist book")
    suoyinhao = Book.objects.get(isbn=suoyinhao)
    if len(Book_Borrow.objects.filter(isbn=suoyinhao,
                                      cardID=cardid, flag=0)) == 0:
        return ret_error("Not exist borrow book record")
    borrow = Book_Borrow.objects.get(isbn=suoyinhao, cardID=cardid, flag=0)
    now_time = datetime.datetime.now()
    borrow.rdate = now_time + datetime.timedelta(days=30)
    borrow.save()
    return ret_succeed()


def guihuan(values):
    suoyinhao = values.get('suoyinhao')
    if not suoyinhao:
        return ret_error("Values not complete")
    if len(Book.objects.filter(isbn=suoyinhao)) == 0:
        return ret_error("Not exist book")
    suoyinhao = Book.objects.get(isbn=suoyinhao)
    if len(Book_Borrow.objects.filter(isbn=suoyinhao, flag=0)) == 0:
        return ret_error("Not exist borrow book record")
    borrow = Book_Borrow.objects.get(isbn=suoyinhao, flag=0)
    now_time = datetime.datetime.now()
    return_time = borrow.rdate.replace(tzinfo=None)
    timedelta = now_time - return_time
    fakuan = 0
    if timedelta.days > 0:
        fakuan = timedelta.days * 0.1
    borrow.rrdate = now_time
    borrow.forfeit = fakuan
    borrow.flag = 1
    borrow.save()
    Book.objects.filter(isbn=values['suoyinhao']).update(status=1)
    ret_values = {"values": {"fakuan": fakuan}}
    return ret_succeed(ret_values)


def book_chaxun(values):
    suoyinhao = values.get('isbn')
    ret_values = {"values": []}
    if not suoyinhao:
        all_book = Book.objects.all()
        for book in all_book:
            if book.status == 0:
                book_status = "借出"
            else:
                book_status = "在馆"
            book_info = {
                'tushubianma': book.isbn,
                'shuming': book.bname,
                'zuozhe': book.bauthor,
                'zhuangtai': book_status,
            }
            ret_values['values'].append(book_info)
        return ret_succeed(ret_values)
    if len(Book.objects.filter(isbn=suoyinhao)) == 0:
        return ret_error("Not exist book")
    book = Book.objects.get(isbn=suoyinhao)
    if book.status == 0:
        book_status = "借出"
    else:
        book_status = "在馆"
    book_info = {
        'tushubianma': book.isbn,
        'shuming': book.bname,
        'zuozhe': book.bauthor,
        'zhuangtai': book_status,
    }
    ret_values['values'].append(book_info)
    return ret_succeed(ret_values)

def consume(values):
    xuehao = values.get('xuehao')
    con_type = values.get('type')
    con_amount =  values.get('amount')
    if not isinstance(con_amount, (int, float)):
        return ret_error("Values type error")
    if con_amount <= 0:
        return ret_error("Money must greater than 0")
    if not xuehao or not con_type:
        return ret_error("Values not complete")
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    card = Card.objects.get(sno=xuehao)
    if card.active == 0:
        return ret_error("Close card")
    new_balance = card.balance - Decimal(str(con_amount))
    if new_balance < 0:
        return ret_error("Not sufficient funds")
    Card.objects.filter(sno=xuehao).update(balance=new_balance)
    con = Consume(cardID=card, con_type=con_type, 
            con_amount=con_amount, con_time=datetime.datetime.now())
    con.save()
    return ret_succeed()

def consume_chaxun(values):
    xuehao = values.get('xuehao')
    ret_values = {"values": []}
    if not xuehao:
        all_consume = Consume.objects.all()
        for consume in all_consume:
            if consume.cardID.sno is None:
                sno = 'null'
            else:
                sno = consume.cardID.sno.sno
            consume_info = {
                'xuehao': sno,
                'type': consume.con_type,
                'amount': str(consume.con_amount),
                'time': timezone.localtime(consume.con_time).strftime('%Y-%m-%d %H:%M:%S')
            }
            ret_values['values'].append(consume_info)
        return ret_succeed(ret_values)
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    cardid = Card.objects.get(sno=xuehao)
    all_consume = Consume.objects.filter(cardID=cardid)
    for consume in all_consume:
        if consume.cardID.sno is None:
            sno = 'null'
        else:
           sno = consume.cardID.sno.sno
        consume_info = {
            'xuehao': sno,
            'type': consume.con_type,
            'amount': str(consume.con_amount),
            'time': timezone.localtime(consume.con_time).strftime('%Y-%m-%d %H:%M:%S')
        }
        ret_values['values'].append(consume_info)
    return ret_succeed(ret_values)


def open_menjin(values):
    xuehao = values.get('xuehao')
    if not xuehao:
        return ret_error('Values not complete')
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    card = Card.objects.get(sno=xuehao)
    if card.active == 0:
        return ret_error("Close card")
    cardid = card
    if len(Guard.objects.filter(cardID=cardid)) != 0:
        Guard.objects.filter(cardID=cardid).update(open=1)
        return ret_succeed()
    guard = Guard(cardID=cardid, open=1)
    guard.save()
    return ret_succeed()


def close_menjin(values):
    xuehao = values.get('xuehao')
    if not xuehao:
        return ret_error('Values not complete')
    if len(Student.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist student")
    xuehao = Student.objects.get(sno=xuehao)
    if len(Card.objects.filter(sno=xuehao)) == 0:
        return ret_error("Not exist card")
    cardid = Card.objects.get(sno=xuehao)
    if len(Guard.objects.filter(cardID=cardid)) == 0:
        return ret_error("Not exist guard")
    Guard.objects.filter(cardID=cardid).update(open=0)
    return ret_succeed()


def menjin_chaxun(values):
    all_guard_record = Guard_record.objects.all()
    ret_values = {"values": []}
    for guard_record in all_guard_record:
        student = Card.objects.get(cardID=guard_record.cardID).sno
        if student == None:
            sno = 'null'
        else:
            sno = student.sno
        guard_record_info = {
            'xuehao': sno,
            'weizhi': guard_record.position,
            'menpaihao': guard_record.gateID,
            'time': timezone.localtime(guard_record.time).strftime('%Y-%m-%d %H:%M:%S')
        }
        ret_values['values'].append(guard_record_info)
    return ret_succeed(ret_values)
