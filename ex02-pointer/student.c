/*
 * student.c
 *
 *  Created on: Feb 15, 2016
 *      Author: jiaziyi
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "student.h"

/**
 * print the student's information
 */
void print_student(student s)
{
	printf("Given name:  %s\n", s.given_name);
    printf("Age:  %d\n", s.age);
    printf("Family name:  %s\n", s.family_name);
	printf("Gender:  %s\n", s.gender);
	printf("Promotion:  %d\n", *s.promotion);
}

/**
 * try to modify the student information
 */
void modify(student s, char *given_name, int age, char *gender)
{
	s.given_name = given_name;
	s.age = age;
	strncpy(s.gender, gender, strlen(gender) + 1);
}

/**
 * try to modify the student information using pointer
 */
void modify_by_pointer(student *s, char *given_name, int age, char *gender)
{
	s->given_name = given_name;
	s->age = age;
	strncpy(s->gender, gender, strlen(gender)+1);
}

student* create_student(char *given_name, char *family_name, int age,
		char* gender, int *promotion)
{

	student s;
	s.given_name = given_name;
	s.family_name = family_name;
	s.age = age;
	strncpy(s.gender, gender, strlen(gender)+1);
	s.promotion = promotion;
	puts("---print inside create_student function---");
	print_student(s);
	puts("---end of print inside");
	return &s;
}
