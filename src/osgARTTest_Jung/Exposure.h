
#ifndef Exposure_H
#define Exposure_H


class Exposure
{
private:
    float index, range, delta;
    
    osg::Uniform * umin;
    osg::Uniform * umax;
  
    void update()
    {
        float min, max;
        min = index - ( range / 2.0 );
        max = index + ( range / 2.0 );

        fprintf(stderr, "Exposure %f, Range %f, Delta %f, Dynamic Range (%f, %f)\n",
                index, range, delta, min, max );

        umin->set( min );
        umax->set( max );
    }

public:
    Exposure()
        : index( 0.5 ), range( 1.0 ), delta( 0.1 ) 
    {
        umin = new osg::Uniform( "min",  0.0f );
        umax = new osg::Uniform( "max",  1.0f );        
        update();
    }
   
    osg::Uniform * getUniformMin()
    {
        return umin;
    }
    osg::Uniform * getUniformMax()
    {
        return umax;
    }
    
    void incIndex()
    {
        index += delta;
        update();
    }
    void decIndex()
    {
        index -= delta;
        update();
    }
    void incRange()
    {
        range += delta;
        update();
    }
    void decRange()
    {
        range -= delta;
        range = range < 0.0 ? 0.0 : range;
        update();
    }
    void incDelta()
    {
        delta += 0.01;
        update();
    }
    void decDelta()
    {
        delta -= 0.01;
        delta = delta < 0.0 ? 0.0 : delta;
        update();
    }

};


#endif