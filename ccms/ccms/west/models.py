from django.db import models

# Create your models here.


class Student(models.Model):
    sno = models.CharField(primary_key=True, max_length=10)
    sname = models.CharField(max_length=20)
    s_class = models.CharField(max_length=50)


class Card(models.Model):
    cardID = models.CharField(max_length=10, primary_key=True)
    balance = models.DecimalField(max_digits=8, decimal_places=2)
    sno = models.OneToOneField('Student', null=True)
    active = models.DecimalField(max_digits=1, decimal_places=0)


class Consume(models.Model):
    cardID = models.ForeignKey('Card')
    con_type = models.CharField(max_length=20)
    con_amount = models.DecimalField(max_digits=5, decimal_places=2)
    con_time = models.DateTimeField()


class Book(models.Model):
    isbn = models.CharField(max_length=20, primary_key=True)
    bno = models.CharField(max_length=20)
    bname = models.CharField(max_length=50)
    bauthor = models.CharField(max_length=20)
    bpress = models.CharField(max_length=50)
    status = models.DecimalField(max_digits=1, decimal_places=0, default=1)


class Book_Borrow(models.Model):
    isbn = models.ForeignKey('Book')
    cardID = models.ForeignKey('Card')
    bdate = models.DateTimeField()
    rdate = models.DateTimeField()
    rrdate = models.DateTimeField(null=True)
    forfeit = models.DecimalField(max_digits=5, decimal_places=2, null=True)
    flag = models.DecimalField(max_digits=1, decimal_places=0, default=0)


class Guard(models.Model):
    cardID = models.OneToOneField('Card', primary_key=True)
    open = models.DecimalField(max_digits=1, decimal_places=0)


class Guard_record(models.Model):
    cardID = models.ForeignKey('Card')
    position = models.CharField(max_length=10)
    gateID = models.CharField(max_length=10)
    time = models.DateTimeField()


class Sign(models.Model):
    cardID = models.OneToOneField('Card', primary_key=True)
    sign_in = models.DecimalField(max_digits=1, decimal_places=0)


class CCMSAdmin(models.Model):
    username = models.CharField(max_length=20, primary_key=True)
    password = models.CharField(max_length=20)
    flag = models.CharField(max_length=20)
