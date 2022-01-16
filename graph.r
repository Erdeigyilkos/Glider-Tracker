args = commandArgs(trailingOnly=TRUE)


name = paste(args[1],".CSV")
name = gsub(" ", "", name, fixed = TRUE)

data = read.csv(name,sep=";");

setwd(args[1])

pdf("grafy.pdf") 

plot(data$altitude, type="l",xaxt="n",  ylab="Y osa",xlab="X osa", col="blue")
axis(1,at=1:nrow(data),labels=data$time)

par(new=T)
plot(data$speed, type="l", yaxt="n",xaxt="n",  ylab="", col="red")
axis(4, col.axis="red")

plot(data$satelites,type="l")

dev.off() 



