import os
import sys
from pylab import *
from pp_utils import *



def mergevector( xbase, ybase, xnew, ynew ):
    
    if xbase is None or ybase is None:
        x = array(xnew)
        y = vstack((ynew,))
        result = {'x':x, 'y':y}
        return result       

    if not any(xbase) or not any(ybase):
        if not any(xnew) or not any(ynew):
            x = []
            y = []
        else:
            x = array(xnew)
            y = vstack((ynew,))
        result = {'x':x, 'y':y}
        return result   

    xbase = array(xbase)
    ybase = array(ybase)
    xnew = array(xnew)
    ynew = array(ynew)

    if all(xbase==xnew):
        x = xbase
        y = vstack((ybase,ynew))
        result = {'x':x, 'y':y}
        return result
    else:
        xrewrite = unique(sort(concatenate((xbase,xnew))))
        
        if len(shape(ybase)) > 1:
            ybaserewrite = zeros((size(ybase,0),size(xrewrite)))
        else:
            ybaserewrite = zeros(shape(xrewrite))
        ynewrewrite = zeros(shape(xrewrite))
       
        for pos in range(size(xrewrite)):
            s = where(xbase==xrewrite[pos])[0]
            if xrewrite[pos] in xbase:
                if len(shape(ybase)) > 1:
                    ybaserewrite[:,pos] = ybase[:,s[0]]
                else:
                    ybaserewrite[pos] = ybase[s[0]]

            s = where(xnew==xrewrite[pos])[0]
            if xrewrite[pos] in xnew:
               ynewrewrite[pos] = ynew[s[0]]
        
        x = xrewrite
        y = vstack((ybaserewrite,ynewrewrite))
        result = {'x':x, 'y':y}
        return result        


def prunevalues( x, y, z = None, lonelyness = 1 ):
    x = array(x)
    y = array(y)
    
    for pos in reversed(range(size(x))):
        if ndim(y) > 1:
            curr = y[:,pos]
        else:
            curr = y[pos]
        nbnull = size(where(curr==0)[0])
        nbnotnull = size(curr) - nbnull
        if (nbnotnull<=lonelyness and size(curr)>1) or nbnotnull==0:
            x = delete(x,[pos],0)
            if ndim(y) > 1:
                y = delete(y,[pos],1)
            else:
                y = delete(y,[pos],0)
            if z is not None:
                if ndim(z) > 1:
                    z = delete(z,[pos],1)
                else:
                    z = delete(z,[pos],0)

    result = {'x':x, 'y':y}
    
    if z is not None:
        result['z'] = z

    return result




