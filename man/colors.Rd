\name{colors}

\docType{methods}

\alias{coltab}
\alias{coltab,SpatRaster-method}
\alias{coltab<-}
\alias{coltab<-,SpatRaster-method}
  
\title{Color table}

\description{
Get the color table(s) associated with a SpatRaster if there are any
}

\usage{
\S4method{coltab}{SpatRaster}(x)

\S4method{coltab}{SpatRaster}(x, layer=1)<-value
}


\arguments{
  \item{x}{SpatRaster}
  \item{layer}{positive integer, the layer number or name}
  \item{value}{a three (red,green,blue) or four (alpha) column data.frame with no more than 256 rows}
}  


\value{
data.frame
}

\examples{
r <- rast(ncol=3, nrow=2, vals=0:5)
coltb <- data.frame(t(col2rgb(rainbow(6, end=.9), alpha=TRUE)))
coltb

plot(r)
coltab(r) <- coltb
plot(r)

tb <- coltab(r)
class(tb)
dim(tb[[1]])
}

\keyword{methods}
\keyword{spatial}

