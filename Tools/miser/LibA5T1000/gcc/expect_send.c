/*
************************************
*       Module : expect_send.c     *
*       Name   : TF                *
*       Date   : 16-01-2008        *
************************************
*/
/*==========================================================================*/
/* Naming conventions                                                       */
/* ~~~~~~~~~~~~~~~~~~                                                       */
/*         Class define : Leading C                                         */
/*        Struct define : Leading S                                         */
/*               Struct : Leading s                                         */
/*               Class  : Leading c                                         */
/*             Constant : Leading K                                         */
/*      Global Variable : Leading g                                         */
/*    Function argument : Leading a                                         */
/*       Local Variable : All lower case                                    */
/*            Byte size : Leading b                                         */
/*            Word size : Leading w                                         */
/*           Dword size : Leading d                                         */
/*              Pointer : Leading p                                         */
/*==========================================================================*/

#include <string.h>
#include "system.h"
#include "expect_send.h"

/*---------------------------------------------------------------------*/
/*
function : to release the allocated T_EXPECT_SEND
input    : T_EXPECT_SEND structure array ptr
           size of array
output   : none
*/
__pcs void expect_send_release(T_EXPECT_SEND ** a_s, DWORD a_n)
{
  T_EXPECT_SEND * ps;
  char **p;

  ps = *a_s;
  if ((ps == NULL) || (a_n == 0))
    return;
  while (a_n--)
  {  
    if (ps->p_send)
      os_free(ps->p_send);
    if (ps->p_abort)
      os_free(ps->p_abort);    
    if (ps->p_expect)
    { 
      p = ps->p_expect; 
      while (*p)
      {
        os_free(*p);
        p++;
      }
      os_free(ps->p_expect);  
    }
    ps++;
  }
  os_free(*a_s);
  *a_s = NULL;
}

/*---------------------------------------------------------------------*/
/*
function : to release the allocated T_EXPECT_SEND
input    : T_EXPECT_SEND structure array ptr
           size of array
output   : none
*/
__pcs T_EXPECT_SEND * expect_send_alloc(T_EXPECT_SEND * a_s,DWORD a_n)
{
  T_EXPECT_SEND * ps,*ops;
  char  *p;
  char  **pp;
  char  **pt;
  DWORD i;
  DWORD size;
  DWORD n;
  
  if ((a_s == NULL) || (a_n == 0))
    return NULL;
  size = sizeof(T_EXPECT_SEND);
  size += 3;
  size &= ~3;
  size *= a_n;
  if ((ops=(T_EXPECT_SEND*)os_malloc(size * a_n)) == NULL)
    return NULL;
  ps = ops;
  n   = a_n;
  memcpy(ps,a_s,size);
  while (n--)
  {
    ps->p_expect = NULL;
    ps->p_send   = NULL;
    ps->p_abort  = NULL;
    ps++;
  }
  ps = ops;
  n   = a_n;
  
  while (n--)
  {
    if (a_s->p_abort)
    {
      if ((p=(char*)os_malloc(strlen(a_s->p_abort)+1))== NULL)
      {
        expect_send_release(&ops,a_n);
        return NULL;
      }
      ps->p_abort = p;
      strcpy(p,a_s->p_abort);
    }
    
    if (a_s->p_send)
    {
      if ((p=(char*)os_malloc(strlen((char*)a_s->p_send)+1))== NULL)
      {
        expect_send_release(&ops,a_n);
        return NULL;
      }
      ps->p_send = (signed char *)p;
      strcpy(p,(char*)a_s->p_send);
    }
      
    if (a_s->p_expect)
    {
      i = sizeof(char*);
      pp = a_s->p_expect;
      while (*pp++)
        i += sizeof(char*);
      
      if ((pt=(char**)os_malloc(i))== NULL)
      {
        expect_send_release(&ops,a_n);
        return NULL;
      }
      memset(pt,0,i);   // set all null
      ps->p_expect = pt;
              
      pp = a_s->p_expect; 
      while (*pp)
      {
        if ((p=(char*)os_malloc(strlen(*pp)+1))== NULL)
        {
          expect_send_release(&ops,a_n);
          return NULL;
        }
        *pt++ = p;
        strcpy(p,*pp);
        pp++;
      }
      *pt = NULL;
    }
    a_s++;
    ps++;
  }
  return ops;
}

